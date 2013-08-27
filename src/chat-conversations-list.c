/*
 * Chat - instant messaging client for GNOME
 * Copyright © 2013 Red Hat, Inc.
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

#include "chat-conversations-list.h"


struct _ChatConversationsListPrivate
{
  GHashTable *accounts;
  TpAccountManager *am;
  TplLogManager *lm;
};


G_DEFINE_TYPE_WITH_PRIVATE (ChatConversationsList, chat_conversations_list, GTK_TYPE_LIST_BOX);


typedef struct _ChatConversationsListGetFilteredEventsData ChatConversationsListGetFilteredEventsData;

struct _ChatConversationsListGetFilteredEventsData
{
  ChatConversationsList *list;
  TpContact *contact;
};


static ChatConversationsListGetFilteredEventsData *
chat_conversations_list_get_filtered_events_data_new (ChatConversationsList *list, TpContact *contact)
{
  ChatConversationsListGetFilteredEventsData *data;

  data = g_slice_new0 (ChatConversationsListGetFilteredEventsData);
  data->list = g_object_ref (list);
  data->contact = g_object_ref (contact);
  return data;
}


static void
chat_conversations_list_get_filtered_events_data_free (ChatConversationsListGetFilteredEventsData *data)
{
  g_object_unref (data->list);
  g_object_unref (data->contact);
  g_slice_free (ChatConversationsListGetFilteredEventsData, data);
}


static gboolean
chat_conversations_list_accounts_key_equal_func (gconstpointer a, gconstpointer b)
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
chat_conversations_list_accounts_value_destroy_func (gpointer data)
{
  g_list_free_full ((GList *) data, g_object_unref);
}


static void
chat_conversations_list_add_row (ChatConversationsList *self, TpContact *contact, TplEvent *event)
{
  GtkWidget *grid;
  GtkWidget *label;
  GtkWidget *row;
  const gchar *alias;
  const gchar *message;
  gchar *markup;

  row = gtk_list_box_row_new ();
  grid = gtk_grid_new ();
  gtk_container_add (GTK_CONTAINER (row), grid);
  gtk_container_add (GTK_CONTAINER (self), row);

  alias = tp_contact_get_alias (contact);
  message = tpl_text_event_get_message (TPL_TEXT_EVENT (event));
  markup = g_markup_printf_escaped ("<b>%s</b>\n%s", alias, message);
  label = gtk_label_new (NULL);
  gtk_label_set_ellipsize (GTK_LABEL (label), PANGO_ELLIPSIZE_END);
  gtk_label_set_markup (GTK_LABEL (label), markup);
  gtk_container_add (GTK_CONTAINER (grid), label);
  g_free (markup);

  gtk_widget_show_all (row);
}


static void
chat_conversations_list_get_filtered_events (GObject *source_object, GAsyncResult *res, gpointer user_data)
{
  ChatConversationsListGetFilteredEventsData *data = (ChatConversationsListGetFilteredEventsData *) user_data;
  ChatConversationsList *self = data->list;
  ChatConversationsListPrivate *priv = self->priv;
  GError *error;
  GList *events;
  TpContact *contact = data->contact;

  error = NULL;
  if (!tpl_log_manager_get_filtered_events_finish (priv->lm, res, &events, &error))
    {
      g_warning ("Unable to get events: %s", error->message);
      g_error_free (error);
      goto out;
    }

  chat_conversations_list_add_row (self, contact, TPL_EVENT (events->data));

 out:
  chat_conversations_list_get_filtered_events_data_free (data);
}


static void
chat_conversations_list_connection_prepare (GObject *source_object, GAsyncResult *res, gpointer user_data)
{
  ChatConversationsList *self = CHAT_CONVERSATIONS_LIST (user_data);
  ChatConversationsListPrivate *priv = self->priv;
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
      ChatConversationsListGetFilteredEventsData *data;
      TpContact *contact;
      TplEntity *entity;

      contact = TP_CONTACT (g_ptr_array_index (contact_list, i));
      entity = tpl_entity_new_from_tp_contact (contact, TPL_ENTITY_CONTACT);
      if (tpl_log_manager_exists (priv->lm, account, entity, TPL_EVENT_MASK_TEXT))
        {
          g_message ("%s", tp_contact_get_alias (contact));
          contacts = g_list_prepend (contacts, g_object_ref (contact));

          data = chat_conversations_list_get_filtered_events_data_new (self, contact);
          tpl_log_manager_get_filtered_events_async (priv->lm,
                                                     account,
                                                     entity,
                                                     TPL_EVENT_MASK_TEXT,
                                                     1,
                                                     NULL,
                                                     NULL,
                                                     chat_conversations_list_get_filtered_events,
                                                     data);
        }

      g_object_unref (entity);
    }

  g_hash_table_insert (priv->accounts, g_object_ref (account), contacts);

 out:
  if (contact_list != NULL)
    g_ptr_array_unref (contact_list);
  g_object_unref (self);
}


static void
chat_conversations_list_account_manager_prepare (GObject *source_object, GAsyncResult *res, gpointer user_data)
{
  ChatConversationsList *self = CHAT_CONVERSATIONS_LIST (user_data);
  ChatConversationsListPrivate *priv = self->priv;
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

      tp_proxy_prepare_async (conn, NULL, chat_conversations_list_connection_prepare, g_object_ref (self));
      g_hash_table_insert (priv->accounts, g_object_ref (account), NULL);
    }

 out:
  g_list_free_full (accounts, g_object_unref);
  g_object_unref (self);
}


static void
chat_conversations_list_dispose (GObject *object)
{
  ChatConversationsList *self = CHAT_CONVERSATIONS_LIST (object);
  ChatConversationsListPrivate *priv = self->priv;

  if (priv->accounts != NULL)
    {
      g_hash_table_unref (priv->accounts);
      priv->accounts = NULL;
    }

  g_clear_object (&priv->am);
  g_clear_object (&priv->lm);

  G_OBJECT_CLASS (chat_conversations_list_parent_class)->dispose (object);
}


static void
chat_conversations_list_init (ChatConversationsList *self)
{
  ChatConversationsListPrivate *priv;

  self->priv = chat_conversations_list_get_instance_private (self);
  priv = self->priv;

  priv->accounts = g_hash_table_new_full (g_direct_hash,
                                          chat_conversations_list_accounts_key_equal_func,
                                          g_object_unref,
                                          chat_conversations_list_accounts_value_destroy_func);

  priv->am = tp_account_manager_dup ();
  tp_proxy_prepare_async (priv->am, NULL, chat_conversations_list_account_manager_prepare, g_object_ref (self));

  priv->lm = tpl_log_manager_dup_singleton ();
}


static void
chat_conversations_list_class_init (ChatConversationsListClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  object_class->dispose = chat_conversations_list_dispose;
}


GtkWidget *
chat_conversations_list_new (void)
{
  return g_object_new (CHAT_TYPE_CONVERSATIONS_LIST, NULL);
}
