/*
 * Copyright (C) 2003-2007 Imendio AB
 * Copyright (C) 2007-2011 Collabora Ltd.
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
 * Authors: Richard Hult <richard@imendio.com>
 *          Martyn Russell <martyn@imendio.com>
 *          Xavier Claessens <xclaesse@gmail.com>
 */

#include "config.h"
#include "empathy-utils.h"

#include <glib/gi18n-lib.h>

const gchar *
empathy_presence_get_default_message (TpConnectionPresenceType presence)
{
  switch (presence)
    {
      case TP_CONNECTION_PRESENCE_TYPE_AVAILABLE:
        return _("Available");
      case TP_CONNECTION_PRESENCE_TYPE_BUSY:
        return _("Busy");
      case TP_CONNECTION_PRESENCE_TYPE_AWAY:
      case TP_CONNECTION_PRESENCE_TYPE_EXTENDED_AWAY:
        return _("Away");
      case TP_CONNECTION_PRESENCE_TYPE_HIDDEN:
        return _("Invisible");
      case TP_CONNECTION_PRESENCE_TYPE_OFFLINE:
        return _("Offline");
      case TP_CONNECTION_PRESENCE_TYPE_UNKNOWN:
        /* translators: presence type is unknown */
        return C_("presence", "Unknown");
      case TP_CONNECTION_PRESENCE_TYPE_UNSET:
      case TP_CONNECTION_PRESENCE_TYPE_ERROR:
      default:
        return NULL;
    }

  return NULL;
}
