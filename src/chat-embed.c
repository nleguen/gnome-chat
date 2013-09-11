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

#include <telepathy-glib/telepathy-glib.h>
#include <telepathy-logger/telepathy-logger.h>

#include "chat-application.h"
#include "chat-conversation-view.h"
#include "chat-conversations-list.h"
#include "chat-embed.h"
#include "chat-main-toolbar.h"
#include "empathy-ui-utils.h"
#include "empathy-utils.h"
#include "polari-fixed-size-frame.h"


struct _ChatEmbedPrivate
{
  GHashTable *conversations;
  GtkSizeGroup *size_group_bottom;
  GtkSizeGroup *size_group_left;
  GtkWidget *conversations_list;
  GtkWidget *current_view;
  GtkWidget *conversations_stack;
  GtkWidget *main_input_area;
  GtkWidget *sidebar_frame;
  GtkWidget *status_area;
  GtkWidget *status_area_nickname;
  GtkWidget *status_area_presence_icon;
  GtkWidget *status_area_presence_message;
  GtkWidget *toolbar;
  TplLogManager *lm;
};

static void chat_embed_buildable_init (GtkBuildableIface *iface);


G_DEFINE_TYPE_WITH_CODE (ChatEmbed, chat_embed, GTK_TYPE_BOX,
                         G_ADD_PRIVATE (ChatEmbed)
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_BUILDABLE,
                                                chat_embed_buildable_init));


static void
chat_embed_ensure_new_line (ChatEmbed *self)
{
  GtkTextBuffer *buffer;
  GtkTextIter iter;
  gint offset;

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (self->priv->current_view));
  gtk_text_buffer_get_end_iter (buffer, &iter);
  offset = gtk_text_iter_get_line_offset (&iter);
  if (offset != 0)
    gtk_text_buffer_insert (buffer, &iter, "\n", -1);
}


static void
chat_embed_get_filtered_events (GObject *source_object, GAsyncResult *res, gpointer user_data)
{
  ChatEmbed *self = CHAT_EMBED (user_data);
  ChatEmbedPrivate *priv = self->priv;
  GError *error;
  GList *events;
  GList *l;
  GtkTextBuffer *buffer;

  gtk_widget_set_sensitive (priv->conversations_list, TRUE);

  error = NULL;
  if (!tpl_log_manager_get_filtered_events_finish (priv->lm, res, &events, &error))
    {
      g_warning ("Unable to get events: %s", error->message);
      g_error_free (error);
      goto out;
    }

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (priv->current_view));

  for (l = events; l != NULL; l = l->next)
    {
      TplEvent *event = TPL_EVENT (l->data);
      GtkTextIter iter;
      const gchar *message;

      chat_embed_ensure_new_line (self);
      gtk_text_buffer_get_end_iter (buffer, &iter);
      message = tpl_text_event_get_message (TPL_TEXT_EVENT (event));
      gtk_text_buffer_insert (buffer, &iter, message, -1);
    }

 out:
  g_object_unref (self);
}


static void
chat_embed_row_activated (ChatEmbed *self, GtkListBoxRow *row)
{
  ChatEmbedPrivate *priv = self->priv;
  GtkWidget *sw;
  TpAccount *account;
  TpConnectionPresenceType presence;
  TpContact *contact;
  TplEntity *entity;
  const gchar *icon_name;
  const gchar *identifier;
  const gchar *nickname;
  gchar *markup = NULL;
  gchar *status_message = NULL;

  contact = g_object_get_data (G_OBJECT (row), "chat-conversations-list-contact");
  account = tp_contact_get_account (contact);

  nickname = tp_account_get_nickname (account);
  markup = g_markup_printf_escaped ("<b>%s</b>", nickname);
  gtk_label_set_markup (GTK_LABEL (priv->status_area_nickname), markup);

  presence = tp_account_get_current_presence (account, NULL, &status_message);
  if (status_message == NULL)
    status_message = g_strdup (empathy_presence_get_default_message (presence));
  icon_name = empathy_icon_name_for_presence (presence);
  gtk_image_set_from_icon_name (GTK_IMAGE (priv->status_area_presence_icon), icon_name, GTK_ICON_SIZE_MENU);
  gtk_label_set_label (GTK_LABEL (priv->status_area_presence_message), status_message);

  identifier = tp_contact_get_identifier (contact);
  priv->current_view = g_hash_table_lookup (priv->conversations, identifier);
  if (priv->current_view != NULL)
    goto out;

  gtk_widget_set_sensitive (priv->conversations_list, FALSE);

  sw = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  gtk_stack_add_named (GTK_STACK (priv->conversations_stack), sw, identifier);
  gtk_widget_show (sw);

  priv->current_view = chat_conversation_view_new ();
  gtk_container_add (GTK_CONTAINER (sw), priv->current_view);
  g_hash_table_insert (priv->conversations, g_strdup (identifier), g_object_ref (priv->current_view));

  entity = tpl_entity_new_from_tp_contact (contact, TPL_ENTITY_CONTACT);
  if (tpl_log_manager_exists (priv->lm, account, entity, TPL_EVENT_MASK_TEXT))
    {
      tpl_log_manager_get_filtered_events_async (priv->lm,
                                                 account,
                                                 entity,
                                                 TPL_EVENT_MASK_TEXT,
                                                 20,
                                                 NULL,
                                                 NULL,
                                                 chat_embed_get_filtered_events,
                                                 g_object_ref (self));
    }

  g_object_unref (entity);

 out:
  gtk_stack_set_visible_child_name (GTK_STACK (priv->conversations_stack), identifier);
  g_free (markup);
  g_free (status_message);
}


static gboolean
chat_embed_select_first_conversation (gpointer user_data)
{
  ChatEmbed *self = CHAT_EMBED (user_data);
  GtkListBoxRow *row;

  row = gtk_list_box_get_row_at_index (GTK_LIST_BOX (self->priv->conversations_list), 0);
  chat_embed_row_activated (self, row);

  return G_SOURCE_REMOVE;
}


static void
chat_embed_conversation_add (ChatEmbed *self, GtkWidget *widget)
{
  static gboolean first_row = TRUE;

  if (!first_row)
    return;

  g_idle_add_full (G_PRIORITY_DEFAULT_IDLE,
                   chat_embed_select_first_conversation,
                   g_object_ref (self),
                   g_object_unref);
  first_row = FALSE;
}


static void
chat_embed_window_added (ChatEmbed *self, GtkWindow *window)
{
  gtk_window_set_titlebar (window, self->priv->toolbar);
}


static void
chat_embed_dispose (GObject *object)
{
  ChatEmbed *self = CHAT_EMBED (object);
  ChatEmbedPrivate *priv = self->priv;

  if (priv->conversations != NULL)
    {
      g_hash_table_unref (priv->conversations);
      priv->conversations = NULL;
    }

  g_clear_object (&priv->size_group_bottom);
  g_clear_object (&priv->size_group_left);
  g_clear_object (&priv->lm);

  G_OBJECT_CLASS (chat_embed_parent_class)->dispose (object);
}


static void
chat_embed_init (ChatEmbed *self)
{
  ChatEmbedPrivate *priv;
  GtkApplication *app;
  GtkWidget *toolbar_left;

  self->priv = chat_embed_get_instance_private (self);
  priv = self->priv;

  gtk_widget_init_template (GTK_WIDGET (self));

  priv->conversations = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_object_unref);

  priv->lm = tpl_log_manager_dup_singleton ();

  app = chat_application_new ();
  g_signal_connect_swapped (app, "window-added", G_CALLBACK (chat_embed_window_added), self);
  g_object_unref (app);

  priv->toolbar = chat_main_toolbar_new ();
  toolbar_left = chat_main_toolbar_get_toolbar_left (CHAT_MAIN_TOOLBAR (priv->toolbar));

  priv->size_group_bottom = gtk_size_group_new (GTK_SIZE_GROUP_VERTICAL);
  gtk_size_group_add_widget (priv->size_group_bottom, priv->status_area);
  gtk_size_group_add_widget (priv->size_group_bottom, priv->main_input_area);

  priv->size_group_left = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
  gtk_size_group_add_widget (priv->size_group_left, priv->sidebar_frame);
  gtk_size_group_add_widget (priv->size_group_left, toolbar_left);

  g_signal_connect_swapped (priv->conversations_list, "add", G_CALLBACK (chat_embed_conversation_add), self);
  g_signal_connect_swapped (priv->conversations_list, "row-activated", G_CALLBACK (chat_embed_row_activated), self);
}


static void
chat_embed_class_init (ChatEmbedClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (class);

  object_class->dispose = chat_embed_dispose;

  g_type_ensure (CHAT_TYPE_CONVERSATIONS_LIST);
  g_type_ensure (POLARI_TYPE_FIXED_SIZE_FRAME);
  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/chat/embed.ui");
  gtk_widget_class_bind_template_child_private (widget_class, ChatEmbed, conversations_list);
  gtk_widget_class_bind_template_child_private (widget_class, ChatEmbed, conversations_stack);
  gtk_widget_class_bind_template_child_private (widget_class, ChatEmbed, main_input_area);
  gtk_widget_class_bind_template_child_private (widget_class, ChatEmbed, sidebar_frame);
  gtk_widget_class_bind_template_child_private (widget_class, ChatEmbed, status_area);
  gtk_widget_class_bind_template_child_private (widget_class, ChatEmbed, status_area_nickname);
  gtk_widget_class_bind_template_child_private (widget_class, ChatEmbed, status_area_presence_icon);
  gtk_widget_class_bind_template_child_private (widget_class, ChatEmbed, status_area_presence_message);
}


static void
chat_embed_buildable_init (GtkBuildableIface *iface)
{
}


GtkWidget *
chat_embed_new (void)
{
  return g_object_new (CHAT_TYPE_EMBED, NULL);
}
