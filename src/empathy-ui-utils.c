/*
 * Copyright (C) 2002-2007 Imendio AB
 * Copyright (C) 2007-2010 Collabora Ltd.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 *
 * Authors: Mikael Hallendal <micke@imendio.com>
 *          Richard Hult <richard@imendio.com>
 *          Martyn Russell <martyn@imendio.com>
 *          Xavier Claessens <xclaesse@gmail.com>
 *          Jonny Lamb <jonny.lamb@collabora.co.uk>
 *          Travis Reitter <travis.reitter@collabora.co.uk>
 *
 *          Part of this file is copied from GtkSourceView (gtksourceiter.c):
 *          Paolo Maggi
 *          Jeroen Zwartepoorte
 */

#include "config.h"
#include "empathy-ui-utils.h"

#include <gtk/gtk.h>

#include "empathy-images.h"

const gchar *
empathy_icon_name_for_presence (TpConnectionPresenceType presence)
{
  switch (presence)
    {
      case TP_CONNECTION_PRESENCE_TYPE_AVAILABLE:
        return EMPATHY_IMAGE_AVAILABLE;
      case TP_CONNECTION_PRESENCE_TYPE_BUSY:
        return EMPATHY_IMAGE_BUSY;
      case TP_CONNECTION_PRESENCE_TYPE_AWAY:
        return EMPATHY_IMAGE_AWAY;
      case TP_CONNECTION_PRESENCE_TYPE_EXTENDED_AWAY:
        if (gtk_icon_theme_has_icon (gtk_icon_theme_get_default (),
                   EMPATHY_IMAGE_EXT_AWAY))
          return EMPATHY_IMAGE_EXT_AWAY;

        /* The 'extended-away' icon is not an official one so we fallback to
         * idle if it's not implemented */
        return EMPATHY_IMAGE_IDLE;
      case TP_CONNECTION_PRESENCE_TYPE_HIDDEN:
        if (gtk_icon_theme_has_icon (gtk_icon_theme_get_default (),
                   EMPATHY_IMAGE_HIDDEN))
          return EMPATHY_IMAGE_HIDDEN;

        /* The 'hidden' icon is not an official one so we fallback to offline if
         * it's not implemented */
        return EMPATHY_IMAGE_OFFLINE;
      case TP_CONNECTION_PRESENCE_TYPE_OFFLINE:
      case TP_CONNECTION_PRESENCE_TYPE_ERROR:
        return EMPATHY_IMAGE_OFFLINE;
      case TP_CONNECTION_PRESENCE_TYPE_UNKNOWN:
        return EMPATHY_IMAGE_PENDING;
      case TP_CONNECTION_PRESENCE_TYPE_UNSET:
      default:
        return NULL;
    }

  return NULL;
}
