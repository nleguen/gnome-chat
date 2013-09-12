/*
 * Chat - instant messaging client for GNOME
 * Copyright © 2013 Yosef Or Boczko <yoseforb@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */


#include "config.h"

#include <glib.h>
#include <telepathy-glib/telepathy-glib.h>
#include <telepathy-logger/telepathy-logger.h>

#include "chat-contacts-list-dialog.h"


struct _ChatContactsListDialogPrivate
{
  GtkWidget *list_box;
  GHashTable *accounts;
  TpAccountManager *am;
};


G_DEFINE_TYPE_WITH_PRIVATE (ChatContactsListDialog, chat_contacts_list_dialog, GTK_TYPE_DIALOG);

static gboolean
chat_contacts_list_dialog_accounts_key_equal_func (gconstpointer a, gconstpointer b)
{
  TpAccount *account_a = TP_ACCOUNT (a);
  TpAccount *account_b = TP_ACCOUNT (b);
  const gchar *path_suffix_a;
  const gchar *path_suffix_b;

  path_suffix_a = tp_account_get_path_suffix (account_a);
  path_suffix_b = tp_account_get_path_suffix (account_b);

  return g_strcmp0 (path_suffix_a, path_suffix_b) == 0;
}


static void
chat_contacts_list_dialog_accounts_value_destroy_func (gpointer data)
{
  g_list_free_full ((GList *) data, g_object_unref);
}


static GtkWidget *
chat_contacts_list_dialog_get_avatar_image (TpContact *contact)
{
  GFile *avatar_file;

  avatar_file = tp_contact_get_avatar_file (contact);
  if (avatar_file)
    {
      GFileInputStream *stream;
      GdkPixbuf *pixbuf;
      GError *error = NULL;

      stream = g_file_read (avatar_file, NULL, &error);
      if (!stream)
        {
          g_warning ("Failed to load avatar: %s", error->message);
          g_error_free (error);
        }
      else
        {
          pixbuf = gdk_pixbuf_new_from_stream (G_INPUT_STREAM (stream), NULL, &error);
          if (!pixbuf)
            {
              g_warning ("Failed to create avatar: %s", error->message);
              g_error_free (error);
            }
          else
            {
              return gtk_image_new_from_pixbuf (pixbuf);
            }
        }
    }

  return gtk_image_new_from_icon_name ("avatar-default-symbolic",
                                       GTK_ICON_SIZE_LARGE_TOOLBAR);
}


static const gchar *
chat_contacts_list_dialog_get_presence_image (TpConnectionPresenceType presence)
{
  switch (presence)
    {
      case TP_CONNECTION_PRESENCE_TYPE_OFFLINE:
        return "user-offline-symbolic";
      case TP_CONNECTION_PRESENCE_TYPE_AVAILABLE:
        return "user-available-symbolic";
      case TP_CONNECTION_PRESENCE_TYPE_AWAY:
        return "user-away-symbolic";
      case TP_CONNECTION_PRESENCE_TYPE_EXTENDED_AWAY:
        return "user-idle-symbolic";
      case TP_CONNECTION_PRESENCE_TYPE_HIDDEN:
        return "user-invisible-symbolic";
      case TP_CONNECTION_PRESENCE_TYPE_BUSY:
        return "user-busy-symbolic";
      default:
        return NULL;
    }
}


static void
chat_contacts_list_dialog_add_row (ChatContactsListDialog *self, TpContact *contact)
{
  ChatContactsListDialogPrivate *priv = self->priv;
  TpConnectionPresenceType presence;
  GtkWidget *row;
  GtkWidget *box;
  GtkWidget *label;
  GtkWidget *image;
  const gchar *state;

  row = gtk_list_box_row_new ();
  box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 6);
  gtk_widget_set_hexpand (box, TRUE);
  gtk_container_set_border_width (GTK_CONTAINER (box), 6);
  gtk_container_add (GTK_CONTAINER (row), box);
  gtk_container_add (GTK_CONTAINER (priv->list_box), row);

  presence = tp_contact_get_presence_type (contact);
  if (presence == TP_CONNECTION_PRESENCE_TYPE_UNSET ||
      presence == TP_CONNECTION_PRESENCE_TYPE_ERROR)
    {
      return;
    }

  image = chat_contacts_list_dialog_get_avatar_image (contact);
  gtk_container_add (GTK_CONTAINER (box), image);

  label = gtk_label_new (tp_contact_get_alias (contact));
  gtk_container_add (GTK_CONTAINER (box), label);

  state = chat_contacts_list_dialog_get_presence_image (presence);
  if (state)
    {
      image = gtk_image_new_from_icon_name (state, GTK_ICON_SIZE_BUTTON);
      gtk_box_pack_end (GTK_BOX (box), image, FALSE, FALSE, 0);
    }

  gtk_widget_show_all (row);
}


static void
chat_contacts_list_dialog_connection_prepare (GObject *source_object, GAsyncResult *res, gpointer user_data)
{
  ChatContactsListDialog *self = CHAT_CONTACTS_LIST_DIALOG (user_data);
  ChatContactsListDialogPrivate *priv = self->priv;
  GError *error;
  GList *contacts = NULL;
  GPtrArray *contact_list = NULL;
  TpAccount *account;
  TpConnection *conn = TP_CONNECTION (source_object);
  TpContactListState state;
  guint i;

  error = NULL;
  if (!tp_proxy_prepare_finish (source_object, res, &error))
    {
      g_warning ("Unable to prepare the connection: %s", error->message);
      g_error_free (error);
      goto out;
    }

  state = tp_connection_get_contact_list_state (conn);
  if (state != TP_CONTACT_LIST_STATE_SUCCESS)
    {
      g_warning ("Value of connection:contact-list-state %p was %d", conn, state);
      goto out;
    }

  account = tp_connection_get_account (conn);
  contact_list = tp_connection_dup_contact_list (conn);
  for (i = 0; i < contact_list->len; i++)
    {
      TpContact *contact;

      contact = TP_CONTACT (g_ptr_array_index (contact_list, i));
      chat_contacts_list_dialog_add_row (self, contact);

      contacts = g_list_prepend (contacts, g_object_ref (contact));
    }

  g_hash_table_insert (priv->accounts, g_object_ref (account), contacts);

 out:
  if (contact_list != NULL)
    g_ptr_array_unref (contact_list);
  g_object_unref (self);
}


static void
chat_contacts_list_dialog_account_manager_prepare (GObject *source_object, GAsyncResult *res, gpointer user_data)
{
  ChatContactsListDialog *self = CHAT_CONTACTS_LIST_DIALOG (user_data);
  ChatContactsListDialogPrivate *priv = self->priv;
  GError *error;
  GList *accounts = NULL;
  GList *l;

  error = NULL;
  if (!tp_proxy_prepare_finish (source_object, res, &error))
    {
      g_warning ("Unable to prepare the account manager: %s", error->message);
      g_error_free (error);
      goto out;
    }

  accounts = tp_account_manager_dup_valid_accounts (priv->am);
  for (l = accounts; l != NULL; l = l->next)
    {
      TpAccount *account = TP_ACCOUNT (l->data);
      TpConnection *conn;
      const gchar *cm_name;

      cm_name = tp_account_get_cm_name (account);
      if (g_strcmp0 (cm_name, "idle") == 0)
        continue;

      conn = tp_account_get_connection (account);
      if (conn == NULL)
        continue;

      tp_proxy_prepare_async (conn, NULL, chat_contacts_list_dialog_connection_prepare, g_object_ref (self));
      g_hash_table_insert (priv->accounts, g_object_ref (account), NULL);
    }

 out:
  g_list_free_full (accounts, g_object_unref);
  g_object_unref (self);
}


static void
chat_contacts_list_dialog_update_header_func (GtkListBoxRow *row, GtkListBoxRow *before, gpointer user_data)
{
  GtkWidget *current;

  if (before == NULL)
    return;

  current = gtk_list_box_row_get_header (row);
  if (current == NULL)
    {
      current = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
      gtk_widget_show (current);
      gtk_list_box_row_set_header (row, current);
    }
}


static void
chat_contacts_list_dialog_dispose (GObject *object)
{
  ChatContactsListDialog *self = CHAT_CONTACTS_LIST_DIALOG (object);
  ChatContactsListDialogPrivate *priv = self->priv;

  if (priv->accounts != NULL)
    {
      g_hash_table_unref (priv->accounts);
      priv->accounts = NULL;
    }

  g_clear_object (&priv->am);

  G_OBJECT_CLASS (chat_contacts_list_dialog_parent_class)->dispose (object);
}


static void
chat_contacts_list_dialog_init (ChatContactsListDialog *self)
{
  ChatContactsListDialogPrivate *priv;

  self->priv = chat_contacts_list_dialog_get_instance_private (self);
  priv = self->priv;

  gtk_widget_init_template (GTK_WIDGET (self));

  gtk_list_box_set_header_func (GTK_LIST_BOX (priv->list_box), chat_contacts_list_dialog_update_header_func, NULL, NULL);

  priv->accounts = g_hash_table_new_full (g_direct_hash,
                                          chat_contacts_list_dialog_accounts_key_equal_func,
                                          g_object_unref,
                                          chat_contacts_list_dialog_accounts_value_destroy_func);

  priv->am = tp_account_manager_dup ();
  tp_proxy_prepare_async (priv->am, NULL, chat_contacts_list_dialog_account_manager_prepare, g_object_ref (self));
}


static void
chat_contacts_list_dialog_class_init (ChatContactsListDialogClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (class);

  object_class->dispose = chat_contacts_list_dialog_dispose;

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/chat/contacts-list-dialog.ui");
  gtk_widget_class_bind_template_child_private (widget_class, ChatContactsListDialog, list_box);
}


GtkWidget *
chat_contacts_list_dialog_new (void)
{
  return g_object_new (CHAT_TYPE_CONTACTS_LIST_DIALOG, NULL);
}
