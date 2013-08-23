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


#ifndef CHAT_MODE_CONTROLLER_H
#define CHAT_MODE_CONTROLLER_H

#include <glib-object.h>

G_BEGIN_DECLS

#define CHAT_TYPE_MODE_CONTROLLER (chat_mode_controller_get_type ())

#define CHAT_MODE_CONTROLLER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
   CHAT_TYPE_MODE_CONTROLLER, ChatModeController))

#define CHAT_MODE_CONTROLLER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
   CHAT_TYPE_MODE_CONTROLLER, ChatModeControllerClass))

#define CHAT_IS_MODE_CONTROLLER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
   CHAT_TYPE_MODE_CONTROLLER))

#define CHAT_IS_MODE_CONTROLLER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
   CHAT_TYPE_MODE_CONTROLLER))

#define CHAT_MODE_CONTROLLER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
   CHAT_TYPE_MODE_CONTROLLER, ChatModeControllerClass))

typedef enum
{
  CHAT_WINDOW_MODE_NONE,
  CHAT_WINDOW_MODE_OVERVIEW
} ChatWindowMode;

typedef struct _ChatModeController        ChatModeController;
typedef struct _ChatModeControllerClass   ChatModeControllerClass;
typedef struct _ChatModeControllerPrivate ChatModeControllerPrivate;

struct _ChatModeController
{
  GObject parent_instance;
  ChatModeControllerPrivate *priv;
};

struct _ChatModeControllerClass
{
  GObjectClass parent_class;

  void (*window_mode_changed)    (ChatModeController *self, ChatWindowMode mode, ChatWindowMode old_mode);
};

GType                chat_mode_controller_get_type               (void) G_GNUC_CONST;

ChatModeController  *chat_mode_controller_dup_singleton          (void);

ChatWindowMode       chat_mode_controller_get_window_mode        (ChatModeController *self);

void                 chat_mode_controller_set_window_mode        (ChatModeController *self,
                                                                  ChatWindowMode      mode);

G_END_DECLS

#endif /* CHAT_MODE_CONTROLLER_H */
