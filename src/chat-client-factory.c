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

#include "chat-client-factory.h"


G_DEFINE_TYPE (ChatClientFactory, chat_client_factory, TP_TYPE_AUTOMATIC_CLIENT_FACTORY);


static GArray *
chat_client_factory_dup_account_features (TpSimpleClientFactory *factory, TpAccount *account)
{
  GArray *features;
  GQuark feature;

  features = TP_SIMPLE_CLIENT_FACTORY_CLASS (chat_client_factory_parent_class)
    ->dup_account_features (factory, account);

  feature = TP_ACCOUNT_FEATURE_CONNECTION;
  g_array_append_val (features, feature);

  return features;
}


static GArray *
chat_client_factory_dup_connection_features (TpSimpleClientFactory *factory, TpConnection *connection)
{
  GArray *features;
  GQuark feature;

  features = TP_SIMPLE_CLIENT_FACTORY_CLASS (chat_client_factory_parent_class)
    ->dup_connection_features (factory, connection);

  feature = TP_CONNECTION_FEATURE_CONTACT_LIST;
  g_array_append_val (features, feature);

  return features;
}


static GArray *
chat_client_factory_dup_contact_features (TpSimpleClientFactory *factory, TpConnection *connection)
{
  GArray *features;
  GQuark feature;

  features = TP_SIMPLE_CLIENT_FACTORY_CLASS (chat_client_factory_parent_class)
    ->dup_contact_features (factory, connection);

  feature = TP_CONTACT_FEATURE_ALIAS;
  g_array_append_val (features, feature);

  feature = TP_CONTACT_FEATURE_AVATAR_DATA;
  g_array_append_val (features, feature);

  feature = TP_CONTACT_FEATURE_AVATAR_TOKEN;
  g_array_append_val (features, feature);

  return features;
}


static GObject *
chat_client_factory_constructor (GType type, guint n_construct_params, GObjectConstructParam *construct_params)
{
  static GObject *self = NULL;

  if (self == NULL)
    {
      self = G_OBJECT_CLASS (chat_client_factory_parent_class)->constructor (type,
                                                                             n_construct_params,
                                                                             construct_params);
      g_object_add_weak_pointer (self, (gpointer) &self);
      return self;
    }

  return g_object_ref (self);
}


static void
chat_client_factory_init (ChatClientFactory *self)
{
}


static void
chat_client_factory_class_init (ChatClientFactoryClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);
  TpSimpleClientFactoryClass *simple_class = TP_SIMPLE_CLIENT_FACTORY_CLASS (class);

  object_class->constructor = chat_client_factory_constructor;
  simple_class->dup_account_features = chat_client_factory_dup_account_features;
  simple_class->dup_connection_features = chat_client_factory_dup_connection_features;
  simple_class->dup_contact_features = chat_client_factory_dup_contact_features;
}


TpSimpleClientFactory *
chat_client_factory_dup_singleton (void)
{
  return g_object_new (CHAT_TYPE_CLIENT_FACTORY, NULL);
}
