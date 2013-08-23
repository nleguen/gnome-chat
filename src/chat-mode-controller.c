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

#include "chat-enums.h"
#include "chat-marshalers.h"
#include "chat-mode-controller.h"


struct _ChatModeControllerPrivate
{
  ChatWindowMode mode;
};

enum
{
  WINDOW_MODE_CHANGED,
  LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };


G_DEFINE_TYPE_WITH_PRIVATE (ChatModeController, chat_mode_controller, G_TYPE_OBJECT);


static GObject *
chat_mode_controller_constructor (GType type, guint n_construct_params, GObjectConstructParam *construct_params)
{
  static GObject *self = NULL;

  if (self == NULL)
    {
      self = G_OBJECT_CLASS (chat_mode_controller_parent_class)->constructor (type,
                                                                              n_construct_params,
                                                                              construct_params);
      g_object_add_weak_pointer (self, (gpointer) &self);
      return self;
    }

  return g_object_ref (self);
}


static void
chat_mode_controller_init (ChatModeController *self)
{
  ChatModeControllerPrivate *priv;

  self->priv = chat_mode_controller_get_instance_private (self);
  priv = self->priv;

  priv->mode = CHAT_WINDOW_MODE_NONE;
}


static void
chat_mode_controller_class_init (ChatModeControllerClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  object_class->constructor = chat_mode_controller_constructor;

  signals[WINDOW_MODE_CHANGED] = g_signal_new ("window-mode-changed",
                                               G_TYPE_FROM_CLASS (class),
                                               G_SIGNAL_RUN_LAST,
                                               G_STRUCT_OFFSET (ChatModeControllerClass,
                                                                window_mode_changed),
                                               NULL, /*accumulator */
                                               NULL, /*accu_data */
                                               _chat_marshal_VOID__ENUM_ENUM,
                                               G_TYPE_NONE,
                                               2,
                                               CHAT_TYPE_WINDOW_MODE,
                                               CHAT_TYPE_WINDOW_MODE);
}


ChatModeController *
chat_mode_controller_dup_singleton (void)
{
  return g_object_new (CHAT_TYPE_MODE_CONTROLLER, NULL);
}


ChatWindowMode
chat_mode_controller_get_window_mode (ChatModeController *self)
{
  return self->priv->mode;
}


void
chat_mode_controller_set_window_mode (ChatModeController *self, ChatWindowMode mode)
{
  ChatModeControllerPrivate *priv = self->priv;
  ChatWindowMode old_mode;

  old_mode = priv->mode;

  if (old_mode == mode)
    return;

  priv->mode = mode;
  g_signal_emit (self, signals[WINDOW_MODE_CHANGED], 0, priv->mode, old_mode);
}
