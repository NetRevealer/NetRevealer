#include <gtk/gtk.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/if_link.h>

#define _GNU_SOURCE     /* To get defns of NI_MAXSERV and NI_MAXHOST */

int main (int   argc, char *argv[]){
    GtkBuilder *builder;
    GObject *GtkToolbar;
    GObject *GtkToolButton;
    GObject *GtkWindow;
    GError *error = NULL;
    gtk_init (&argc, &argv);

    /* Construct a GtkBuilder instance and load our UI description */
    builder = gtk_builder_new ();
    if (gtk_builder_add_from_file (builder, "starter.ui", &error) == 0)
        {
        g_printerr ("Error loading file: %s\n", error->message);
        g_clear_error (&error);
        return 1;
        }

    /* Connect signal handlers to the constructed widgets. */
    GtkWindow = gtk_builder_get_object (builder, "starter");
    // GtkToolbar = gtk_builder_get_object(builder, "toolBar");
    // GtkToolButton = gtk_builder_get_object(builder, "toolButtonAllow");
    // g_signal_connect (GtkToolButton, "clicked", G_CALLBACK (print_hello), NULL);
    // g_signal_connect (GtkWindow, "destroy", G_CALLBACK (gtk_main_quit), NULL);
    
    gtk_main ();

    return 0;
}
