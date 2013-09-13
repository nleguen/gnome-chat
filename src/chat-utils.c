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

#include <gdk-pixbuf/gdk-pixbuf.h>

#include "chat-constants.h"
#include "chat-utils.h"


static void
chat_utils_get_contact_avatar_pixbuf (GObject *source_object, GAsyncResult *res, gpointer user_data)
{
  GTask *task = G_TASK (user_data);
  GdkPixbuf *pixbuf;
  GError *error;
  GtkWidget *image;

  error = NULL;
  pixbuf = gdk_pixbuf_new_from_stream_finish (res, &error);
  if (error != NULL)
    {
      g_task_return_error (task, error);
      goto out;
    }

  image = gtk_image_new_from_pixbuf (pixbuf);
  g_task_return_pointer (task, g_object_ref_sink (image), g_object_unref);

 out:
  g_object_unref (task);
}

static void
chat_utils_get_contact_avatar_file_read (GObject *source_object, GAsyncResult *res, gpointer user_data)
{
  GFile *file = G_FILE (source_object);
  GTask *task = G_TASK (user_data);
  GCancellable *cancellable;
  GFileInputStream *stream;
  GError *error;

  error = NULL;
  stream = g_file_read_finish (file, res, &error);
  if (error != NULL)
    {
      g_task_return_error (task, error);
      goto out;
    }

  cancellable = g_task_get_cancellable (task);
  gdk_pixbuf_new_from_stream_at_scale_async (G_INPUT_STREAM (stream),
                                             CHAT_AVATAR_SIZE,
                                             CHAT_AVATAR_SIZE,
                                             TRUE,
                                             cancellable,
                                             chat_utils_get_contact_avatar_pixbuf,
                                             g_object_ref (task));

 out:
  g_object_unref (task);
}


void
chat_utils_get_contact_avatar (TpContact *contact,
                               GCancellable *cancellable,
                               GAsyncReadyCallback callback,
                               gpointer user_data)
{
  GFile *file;
  GTask *task;

  task = g_task_new (contact, cancellable, callback, user_data);
  g_task_set_check_cancellable (task, TRUE);
  g_task_set_source_tag (task, chat_utils_get_contact_avatar);

  file = tp_contact_get_avatar_file (contact);
  if (file == NULL)
    {
      GtkImage *image;

      image = chat_utils_get_contact_avatar_default ();
      g_task_return_pointer (task, g_object_ref_sink (image), g_object_unref);
      goto out;
    }

  g_file_read_async (file,
                     G_PRIORITY_DEFAULT,
                     cancellable,
                     chat_utils_get_contact_avatar_file_read,
                     g_object_ref (task));

 out:
  g_object_unref (task);
}


GtkImage *
chat_utils_get_contact_avatar_finish (TpContact *contact, GAsyncResult *res, GError **error)
{
  GTask *task = G_TASK (res);

  g_return_val_if_fail (g_task_is_valid (res, contact), NULL);
  g_return_val_if_fail (g_task_get_source_tag (task) == chat_utils_get_contact_avatar, NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  return g_task_propagate_pointer (task, error);
}


GtkImage *
chat_utils_get_contact_avatar_default (void)
{
  GtkWidget *image;

  image = gtk_image_new_from_icon_name ("avatar-default-symbolic", GTK_ICON_SIZE_INVALID);
  gtk_image_set_pixel_size (GTK_IMAGE (image), CHAT_AVATAR_SIZE);
  return GTK_IMAGE (image);
}
