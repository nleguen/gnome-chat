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

#include "chat-constants.h"
#include "chat-manager.h"


struct _ChatManagerPrivate
{
  TpBaseClient *handler;
};


G_DEFINE_TYPE_WITH_PRIVATE (ChatManager, chat_manager, G_TYPE_OBJECT);


static void
chat_manager_handle_channels (TpSimpleHandler *handler,
                              TpAccount *account,
                              TpConnection *connection,
                              GList *channels,
                              GList *requests_satisfied,
                              gint64 user_action_time,
                              TpHandleChannelsContext *context,
                              gpointer user_data)
{
  GError *error;
  const gchar *protocol;

  protocol = tp_account_get_protocol_name (account);
  if (g_strcmp0 (protocol, CHAT_PROTOCOL_IRC) == 0)
    {
      error = NULL;
      g_set_error (&error, TP_ERROR, TP_ERROR_NOT_IMPLEMENTED, "IRC is not implemented");
      tp_handle_channels_context_fail (context, error);
      g_error_free (error);
      return;
    }

  tp_handle_channels_context_accept (context);
}


static GObject *
chat_manager_constructor (GType type, guint n_construct_params, GObjectConstructParam *construct_params)
{
  static GObject *self = NULL;

  if (self == NULL)
    {
      self = G_OBJECT_CLASS (chat_manager_parent_class)->constructor (type, n_construct_params, construct_params);
      g_object_add_weak_pointer (self, (gpointer) &self);
      return self;
    }

  return g_object_ref (self);
}


static void
chat_manager_dispose (GObject *object)
{
  ChatManager *self = CHAT_MANAGER (object);
  ChatManagerPrivate *priv = self->priv;

  g_clear_object (&priv->handler);

  G_OBJECT_CLASS (chat_manager_parent_class)->dispose (object);
}


static void
chat_manager_init (ChatManager *self)
{
  ChatManagerPrivate *priv;
  GError *error;
  GHashTable *filter;
  TpAccountManager *am;

  self->priv = chat_manager_get_instance_private (self);
  priv = self->priv;

  am = tp_account_manager_dup ();
  priv->handler = tp_simple_handler_new_with_am (am,
                                                 FALSE,
                                                 FALSE,
                                                 PACKAGE_NAME,
                                                 FALSE,
                                                 chat_manager_handle_channels,
                                                 self,
                                                 NULL);
  g_object_unref (am);

  filter = tp_asv_new (TP_PROP_CHANNEL_CHANNEL_TYPE, G_TYPE_STRING, TP_IFACE_CHANNEL_TYPE_TEXT,
                       TP_PROP_CHANNEL_TARGET_HANDLE_TYPE, TP_TYPE_HANDLE, TP_HANDLE_TYPE_CONTACT,
                       NULL);
  tp_base_client_take_handler_filter (priv->handler, filter);

  error = NULL;
  if (!tp_base_client_register (priv->handler, &error))
    {
      g_critical ("Unable to register text handler: %s", error->message);
      g_error_free (error);
    }
}


static void
chat_manager_class_init (ChatManagerClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  object_class->constructor = chat_manager_constructor;
  object_class->dispose = chat_manager_dispose;
}


ChatManager *
chat_manager_dup_singleton (void)
{
  return g_object_new (CHAT_TYPE_MANAGER, NULL);
}
