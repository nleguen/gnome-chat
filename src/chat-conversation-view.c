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

#include "chat-conversation-view.h"


static void chat_conversation_view_buildable_init (GtkBuildableIface *iface);


G_DEFINE_TYPE_WITH_CODE (ChatConversationView, chat_conversation_view, GTK_TYPE_TEXT_VIEW,
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_BUILDABLE,
                                                chat_conversation_view_buildable_init));


static void
chat_conversation_get_preferred_width (GtkWidget *widget, gint *minimum_width, gint *natural_width)
{
  if (minimum_width)
    *minimum_width = 1;

  if (natural_width)
    *natural_width = 1;
}


static void
chat_conversation_view_init (ChatConversationView *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}


static void
chat_conversation_view_class_init (ChatConversationViewClass *class)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (class);

  widget_class->get_preferred_width = chat_conversation_get_preferred_width;

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/chat/conversation-view.ui");
}


static void
chat_conversation_view_buildable_init (GtkBuildableIface *iface)
{
}


GtkWidget *
chat_conversation_view_new (void)
{
  return g_object_new (CHAT_TYPE_CONVERSATION_VIEW, NULL);
}
