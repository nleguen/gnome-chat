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

#include "chat-settings.h"


G_DEFINE_TYPE (ChatSettings, chat_settings, G_TYPE_SETTINGS);


static GObject *
chat_settings_constructor (GType                  type,
                           guint                  n_construct_params,
                           GObjectConstructParam *construct_params)
{
  static GObject *self = NULL;

  if (self == NULL)
    {
      self = G_OBJECT_CLASS (chat_settings_parent_class)->constructor (type, n_construct_params, construct_params);
      g_object_add_weak_pointer (self, (gpointer) &self);
      return self;
    }

  return g_object_ref (self);
}


static void
chat_settings_init (ChatSettings *self)
{
}


static void
chat_settings_class_init (ChatSettingsClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);
  object_class->constructor = chat_settings_constructor;
}


GSettings *
chat_settings_dup_singleton (void)
{
  return g_object_new (CHAT_TYPE_SETTINGS, "schema-id", "org.gnome.chat", NULL);
}
