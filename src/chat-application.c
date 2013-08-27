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

#include <gio/gio.h>
#include <glib.h>
#include <telepathy-glib/telepathy-glib.h>

#include "chat-application.h"
#include "chat-client-factory.h"
#include "chat-main-window.h"
#include "chat-mode-controller.h"
#include "chat-resources.h"


struct _ChatApplicationPrivate
{
  ChatModeController *mode_cntrlr;
  GResource *resource;
  GtkWidget *main_window;
};


G_DEFINE_TYPE_WITH_PRIVATE (ChatApplication, chat_application, GTK_TYPE_APPLICATION)


static void
chat_application_quit (ChatApplication *self, GVariant *parameter)
{
  gtk_widget_destroy (self->priv->main_window);
}


static void
chat_application_window_mode_changed (ChatApplication *self, ChatWindowMode mode, ChatWindowMode old_mode)
{
}


static void
chat_application_activate (GApplication *application)
{
  ChatApplication *self = CHAT_APPLICATION (application);
  ChatApplicationPrivate *priv = self->priv;

  gtk_window_present (GTK_WINDOW (priv->main_window));
}


static void
chat_application_startup (GApplication *application)
{
  ChatApplication *self = CHAT_APPLICATION (application);
  ChatApplicationPrivate *priv = self->priv;
  GMenu *menu;
  GSimpleAction *action;
  GtkBuilder *builder;
  TpAccountManager *am;
  TpSimpleClientFactory *factory;

  G_APPLICATION_CLASS (chat_application_parent_class)->startup (application);

  factory = chat_client_factory_dup_singleton ();
  am = tp_account_manager_new_with_factory (factory);
  tp_account_manager_set_default (am);
  g_object_unref (factory);
  g_object_unref (am);

  priv->resource = chat_get_resource ();
  g_resources_register (priv->resource);

  priv->mode_cntrlr = chat_mode_controller_dup_singleton ();

  action = g_simple_action_new ("about", NULL);
  g_signal_connect_swapped (action, "activate", G_CALLBACK (chat_main_window_show_about), priv->main_window);
  g_action_map_add_action (G_ACTION_MAP (self), G_ACTION (action));
  g_object_unref (action);

  action = g_simple_action_new ("quit", NULL);
  g_signal_connect_swapped (action, "activate", G_CALLBACK (chat_application_quit), self);
  g_action_map_add_action (G_ACTION_MAP (self), G_ACTION (action));
  g_object_unref (action);

  g_signal_connect_swapped (priv->mode_cntrlr,
                            "window-mode-changed",
                            G_CALLBACK (chat_application_window_mode_changed),
                            self);

  builder = gtk_builder_new ();
  gtk_builder_add_from_resource (builder, "/org/gnome/chat/app-menu.ui", NULL);

  menu = G_MENU (gtk_builder_get_object (builder, "app-menu"));
  gtk_application_set_app_menu (GTK_APPLICATION (self), G_MENU_MODEL (menu));
  g_object_unref (builder);

  gtk_application_add_accelerator (GTK_APPLICATION (self), "<Primary>q", "app.quit", NULL);

  priv->main_window = chat_main_window_new (GTK_APPLICATION (self));
  chat_mode_controller_set_window_mode (priv->mode_cntrlr, CHAT_WINDOW_MODE_OVERVIEW);
}


static GObject *
chat_application_constructor (GType type, guint n_construct_params, GObjectConstructParam *construct_params)
{
  static GObject *self = NULL;

  if (self == NULL)
    {
      self = G_OBJECT_CLASS (chat_application_parent_class)->constructor (type,
                                                                          n_construct_params,
                                                                          construct_params);
      g_object_add_weak_pointer (self, (gpointer) &self);
      return self;
    }

  return g_object_ref (self);
}


static void
chat_application_dispose (GObject *object)
{
  ChatApplication *self = CHAT_APPLICATION (object);
  ChatApplicationPrivate *priv = self->priv;

  if (priv->resource != NULL)
    {
      g_resources_unregister (priv->resource);
      g_resource_unref (priv->resource);
      priv->resource = NULL;
    }

  g_clear_object (&priv->mode_cntrlr);

  G_OBJECT_CLASS (chat_application_parent_class)->dispose (object);
}


static void
chat_application_init (ChatApplication *self)
{
  self->priv = chat_application_get_instance_private (self);
}


static void
chat_application_class_init (ChatApplicationClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);
  GApplicationClass *application_class = G_APPLICATION_CLASS (class);

  object_class->constructor = chat_application_constructor;
  object_class->dispose = chat_application_dispose;
  application_class->activate = chat_application_activate;
  application_class->startup = chat_application_startup;
}


GtkApplication *
chat_application_new (void)
{
  return g_object_new (CHAT_TYPE_APPLICATION,
                       "application-id", "org.gnome." PACKAGE_NAME,
                       "flags", G_APPLICATION_FLAGS_NONE,
                       NULL);
}
