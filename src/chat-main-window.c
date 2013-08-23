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
#include <glib/gi18n.h>

#include "chat-about-data.h"
#include "chat-embed.h"
#include "chat-main-window.h"
#include "chat-settings.h"


struct _ChatMainWindowPrivate
{
  GSettings *settings;
  guint configure_id;
};


G_DEFINE_TYPE_WITH_PRIVATE (ChatMainWindow, chat_main_window, GTK_TYPE_APPLICATION_WINDOW);


enum
{
  CONFIGURE_ID_TIMEOUT = 100, /* ms */
  WINDOW_MIN_HEIGHT = 600,
  WINDOW_MIN_WIDTH = 640,
};


static void
chat_main_window_save_geometry (ChatMainWindow *self)
{
  ChatMainWindowPrivate *priv = self->priv;
  GVariant *variant;
  GdkWindow *window;
  GdkWindowState state;
  gint32 position[2];
  gint32 size[2];

  window = gtk_widget_get_window (GTK_WIDGET (self));
  state = gdk_window_get_state (window);
  if (state & GDK_WINDOW_STATE_MAXIMIZED)
    return;

  gtk_window_get_size (GTK_WINDOW (self), (gint *) &size[0], (gint *) &size[1]);
  variant = g_variant_new_fixed_array (G_VARIANT_TYPE_INT32, size, 2, sizeof (size[0]));
  g_settings_set_value (priv->settings, "window-size", variant);

  gtk_window_get_position (GTK_WINDOW (self), (gint *) &position[0], (gint *) &position[1]);
  variant = g_variant_new_fixed_array (G_VARIANT_TYPE_INT32, position, 2, sizeof (position[0]));
  g_settings_set_value (priv->settings, "window-position", variant);
}


static gboolean
chat_main_window_configure_id_timeout (gpointer user_data)
{
  ChatMainWindow *self = CHAT_MAIN_WINDOW (user_data);

  chat_main_window_save_geometry (self);
  return G_SOURCE_REMOVE;
}


static gboolean
chat_main_window_configure_event (GtkWidget *widget, GdkEventConfigure *event)
{
  ChatMainWindow *self = CHAT_MAIN_WINDOW (widget);
  ChatMainWindowPrivate *priv = self->priv;
  gboolean ret_val;

  ret_val = GTK_WIDGET_CLASS (chat_main_window_parent_class)->configure_event (widget, event);

  if (priv->configure_id != 0)
    {
      g_source_remove (priv->configure_id);
      priv->configure_id = 0;
    }

  priv->configure_id = g_timeout_add (CONFIGURE_ID_TIMEOUT, chat_main_window_configure_id_timeout, self);
  return ret_val;
}


static gboolean
chat_main_window_delete_event (GtkWidget *widget, GdkEventAny *event)
{
  ChatMainWindow *self = CHAT_MAIN_WINDOW (widget);
  ChatMainWindowPrivate *priv = self->priv;

  if (priv->configure_id != 0)
    {
      g_source_remove (priv->configure_id);
      priv->configure_id = 0;
    }

  chat_main_window_save_geometry (self);
  return GDK_EVENT_PROPAGATE;
}


static gboolean
chat_main_window_window_state_event (GtkWidget *widget, GdkEventWindowState *event)
{
  ChatMainWindow *self = CHAT_MAIN_WINDOW (widget);
  GdkWindow *window;
  GdkWindowState state;
  gboolean maximized;
  gboolean ret_val;

  ret_val = GTK_WIDGET_CLASS (chat_main_window_parent_class)->window_state_event (widget, event);

  window = gtk_widget_get_window (widget);
  state = gdk_window_get_state (window);

  maximized = (state & GDK_WINDOW_STATE_MAXIMIZED);
  g_settings_set_boolean (self->priv->settings, "window-maximized", maximized);

  return ret_val;
}


static void
chat_main_window_dispose (GObject *object)
{
  ChatMainWindow *self = CHAT_MAIN_WINDOW (object);
  ChatMainWindowPrivate *priv = self->priv;

  g_clear_object (&priv->settings);

  if (priv->configure_id != 0)
    {
      g_source_remove (priv->configure_id);
      priv->configure_id = 0;
    }

  G_OBJECT_CLASS (chat_main_window_parent_class)->dispose (object);
}


static void
chat_main_window_init (ChatMainWindow *self)
{
  ChatMainWindowPrivate *priv;
  GVariant *variant;
  gboolean maximized;
  const gint32 *position;
  const gint32 *size;
  gsize n_elements;

  self->priv = chat_main_window_get_instance_private (self);
  priv = self->priv;

  gtk_widget_init_template (GTK_WIDGET (self));

  priv->settings = chat_settings_dup_singleton ();

  variant = g_settings_get_value (priv->settings, "window-size");
  size = g_variant_get_fixed_array (variant, &n_elements, sizeof (gint32));
  if (n_elements == 2)
    gtk_window_set_default_size (GTK_WINDOW (self), size[0], size[1]);
  g_variant_unref (variant);

  variant = g_settings_get_value (priv->settings, "window-position");
  position = g_variant_get_fixed_array (variant, &n_elements, sizeof (gint32));
  if (n_elements == 2)
    gtk_window_move (GTK_WINDOW (self), position[0], position[1]);
  g_variant_unref (variant);

  maximized = g_settings_get_boolean (priv->settings, "window-maximized");
  if (maximized)
    gtk_window_maximize (GTK_WINDOW (self));
}


static void
chat_main_window_class_init (ChatMainWindowClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (class);

  object_class->dispose = chat_main_window_dispose;
  widget_class->configure_event = chat_main_window_configure_event;
  widget_class->delete_event = chat_main_window_delete_event;
  widget_class->window_state_event = chat_main_window_window_state_event;

  g_type_ensure (CHAT_TYPE_EMBED);
  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/chat/main-window.ui");
}


GtkWidget *
chat_main_window_new (GtkApplication *application)
{
  g_return_val_if_fail (GTK_IS_APPLICATION (application), NULL);

  return g_object_new (CHAT_TYPE_MAIN_WINDOW,
                       "width_request", WINDOW_MIN_WIDTH,
                       "height_request", WINDOW_MIN_HEIGHT,
                       "application", application,
                       "title", _(PACKAGE_NAME),
                       NULL);
}


void
chat_main_window_show_about (ChatMainWindow *self)
{
  GtkWidget *about_dialog;

  about_dialog = gtk_about_dialog_new ();
  gtk_window_set_modal (GTK_WINDOW (about_dialog), TRUE);
  gtk_window_set_transient_for (GTK_WINDOW (about_dialog), GTK_WINDOW (self));

  gtk_about_dialog_set_artists (GTK_ABOUT_DIALOG (about_dialog), CHAT_ARTISTS);
  gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG (about_dialog), CHAT_AUTHORS);
  gtk_about_dialog_set_comments (GTK_ABOUT_DIALOG (about_dialog),
                                 _("Instant messaging client for GNOME"));
  gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG (about_dialog), "Copyright © 2013 Red Hat, Inc.");
  gtk_about_dialog_set_license_type (GTK_ABOUT_DIALOG (about_dialog), GTK_LICENSE_LGPL_2_1);
  gtk_about_dialog_set_logo_icon_name (GTK_ABOUT_DIALOG (about_dialog), PACKAGE_TARNAME);
  gtk_about_dialog_set_program_name (GTK_ABOUT_DIALOG (about_dialog), _(PACKAGE_NAME));
  gtk_about_dialog_set_version (GTK_ABOUT_DIALOG (about_dialog), _(PACKAGE_VERSION));
  gtk_about_dialog_set_website (GTK_ABOUT_DIALOG (about_dialog), PACKAGE_URL);
  gtk_about_dialog_set_wrap_license (GTK_ABOUT_DIALOG (about_dialog), TRUE);

  /* Translators: Put your names here */
  gtk_about_dialog_set_translator_credits (GTK_ABOUT_DIALOG (about_dialog), _("translator-credits"));

  gtk_widget_show (about_dialog);
  g_signal_connect (about_dialog, "response", G_CALLBACK (gtk_widget_destroy), NULL);
}
