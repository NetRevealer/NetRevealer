#include <gtk/gtk.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/if_link.h>
#include "main.c"

#define _GNU_SOURCE     /* To get defns of NI_MAXSERV and NI_MAXHOST */

// static GtkWidget * create_row (const gchar *text){
//   GtkWidget *row, *handle, *box, *label, *image;

//   row = gtk_list_box_row_new ();

//   box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
//   g_object_set (box, "margin-start", 10, "margin-end", 10, NULL);

//   handle = gtk_event_box_new ();
//   gtk_container_add (GTK_CONTAINER (box), handle);

//   label = gtk_label_new (text);
//   gtk_container_add_with_properties (GTK_CONTAINER (box), label, "expand", TRUE, NULL);
//   return row;
// }

enum interface_cols{
    COL_INDEX,
    COL_TYPE,
    COL_TX_PACKETS,
    COL_RX_PACKETS,
    COL_TX_BYTES,
    COL_RX_BYTES,
    NUM_COLS_INTERFACES,
};

int argc;
char *argv[];
gchar *selected_type;
gboolean network_rules_backup = 1;
GObject *gtkWindowStarter;
GtkTreeSelection *selection;
GtkListStore  *store;
GtkTreeModel *model;
GtkTreeIter iter;
GtkWidget *view;



static GtkTreeModel * create_and_fill_model (void){
    int i = 0;

    
    store = gtk_list_store_new (NUM_COLS_INTERFACES,G_TYPE_INT, G_TYPE_STRING , G_TYPE_INT, G_TYPE_INT,G_TYPE_INT ,G_TYPE_INT);


    struct ifaddrs *ifaddr, *ifa;
    int family, s;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    /* Walk through linked list, maintaining head pointer so we
        can free list later */
    
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        family = ifa->ifa_addr->sa_family;
        if (family == AF_PACKET && ifa->ifa_data != NULL) {
            struct rtnl_link_stats *stats = ifa->ifa_data;
            gtk_list_store_append (store, &iter);
            gtk_list_store_set (store, &iter,
                        COL_INDEX, i,
                        COL_TYPE, ifa->ifa_name,
                        COL_TX_PACKETS, stats->tx_packets,
                        COL_RX_PACKETS, stats->rx_packets,
                        COL_TX_BYTES, stats->tx_bytes,
                        COL_RX_BYTES, stats->rx_bytes,
                        -1);
            
        }
        i++;
    }
            

    return GTK_TREE_MODEL (store);
};

static GtkWidget * create_view_and_model (void){
    
    GtkCellRenderer     *renderer;
    GtkTreeModel        *model;
    GtkWidget           *view;
    GtkTreeViewColumn *column;

    view = gtk_tree_view_new ();

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "N°",  
                                                renderer,
                                                "text", COL_INDEX,
                                                NULL);
                                                    

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Type",  
                                                renderer,
                                                "text", COL_TYPE,
                                                NULL);

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "TX Packets",  
                                                renderer,
                                                "text", COL_TX_PACKETS,
                                                NULL);

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "RX Packets",  
                                                renderer,
                                                "text", COL_TX_PACKETS,
                                                NULL);
    
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "TX Bytes",  
                                                renderer,
                                                "text", COL_TX_BYTES,
                                                NULL);

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "RX Bytes",  
                                                renderer,
                                                "text", COL_RX_BYTES,
                                                NULL);

    model = create_and_fill_model ();

    gtk_tree_view_set_model (GTK_TREE_VIEW (view), model);

    for (int i =0; i < 6; i++){
        column = gtk_tree_view_get_column(view, i);
        gtk_tree_view_column_set_sort_column_id(column, i);
    }

    /* The tree view has acquired its own reference to the
    *  model, so we can drop ours. That way the model will
    *  be freed automatically when the tree view is destroyed */

    g_object_unref (model);

    return view;
};

gboolean view_selection_func (GtkTreeSelection *selection,
                       GtkTreeModel     *model,
                       GtkTreePath      *path,
                       gboolean          path_currently_selected,
                       gpointer          userdata){
    GtkTreeIter iter;

    if (gtk_tree_model_get_iter(model, &iter, path)){
        gchar *type;

        gtk_tree_model_get(model, &iter, COL_TYPE, &type, -1);

        if (!path_currently_selected){
            selected_type = type;
            g_print ("%s is going to be selected.\n", type);
        }
        else{
            g_print ("%s is going to be unselected.\n", type);
        }

    }

    return TRUE; /* allow selection state to change */
}

void gtkWarningContinue_clicked(GtkWidget *widget, gpointer data, GtkWindow *window) {
    GError *error = NULL;
    GtkBuilder *gtkmainWindow;
    GObject *mainWindow;


    
    // gtkmainWindow = gtk_builder_new ();
    // if (gtk_builder_add_from_file (gtkmainWindow, "main.ui", &error) == 0){
    //     g_printerr ("Error loading file: %s\n", error->message);
    //     g_clear_error (&error);
    //     return 1;
    // }
    // mainWindow = gtk_builder_get_object (gtkmainWindow, "mainWindow");
    gtk_window_close(window);
    gtk_window_close(gtkWindowStarter);
    start(argc, argv);
    gtk_main();
    gtk_main_quit();
    
    
    


}

void gtkWarningCancel_clicked(GtkWidget *widget, gpointer data, GtkWindow *window) {
    gtk_widget_set_sensitive(gtkWindowStarter, TRUE);
    gtk_window_close(window);

}
void gtkErrorOk_clicked(GtkWidget *widget, gpointer data, GtkWidget *window) {
    gtk_window_close (window);
    gtk_widget_set_sensitive(gtkWindowStarter, TRUE);
}

void gtkWarningErrorQuit_clicked(GtkWidget *widget, gpointer data){
    gtk_window_close(widget);
    gtk_widget_set_sensitive(gtkWindowStarter, TRUE);
    
}

void gtkToolBarSelect_clicked(GtkWidget *widget, gpointer data) {
    GtkBuilder *warningBuilder;
    GtkBuilder *errorBuilder;
    GtkWindow *gtkWarningBackup;
    GObject *gtkWarningContinue;
    GObject *gtkErrorOk;
    GtkWidget *gtkErrorInterface;
    GObject *gtkWarningCancel;
    GError *error = NULL;
    
    gtk_widget_set_sensitive(gtkWindowStarter, FALSE);
    if (selected_type == NULL){
        errorBuilder = gtk_builder_new ();
        if (gtk_builder_add_from_file (errorBuilder, "error_interface.ui", &error) == 0){
            g_printerr ("Error loading file: %s\n", error->message);
            g_clear_error (&error);
            return 1;
        }
        gtkErrorInterface = gtk_builder_get_object (errorBuilder, "gtkErrorInterface");
        gtkErrorOk = gtk_builder_get_object (errorBuilder, "gtkErrorOk");
        g_signal_connect (gtkErrorOk, "clicked", gtkErrorOk_clicked, gtkErrorInterface);
        g_signal_connect (gtkErrorInterface, "destroy", gtkWarningErrorQuit_clicked, NULL);

    } else {
        warningBuilder = gtk_builder_new ();
        if (gtk_builder_add_from_file (warningBuilder, "warning_backup.ui", &error) == 0){
            g_printerr ("Error loading file: %s\n", error->message);
            g_clear_error (&error);
            return 1;
        }

        gtkWarningBackup = gtk_builder_get_object (warningBuilder, "gtkWarningBackup");
        gtkWarningContinue = gtk_builder_get_object (warningBuilder, "gtkWarningContinue");
        gtkWarningCancel = gtk_builder_get_object (warningBuilder, "gtkWarningCancel");
        g_signal_connect (gtkWarningBackup, "destroy", gtkWarningErrorQuit_clicked, NULL);
        g_signal_connect (gtkWarningContinue, "clicked", gtkWarningContinue_clicked, gtkWarningBackup);
        g_signal_connect (gtkWarningCancel, "clicked", gtkWarningCancel_clicked, gtkWarningBackup);

    }

}



void gtkToolBarBackup_clicked(GtkWidget *widget, gpointer data, GtkWindow *window){
    GtkWidget *dialog;
    GtkFileChooser *chooser;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
    gint res;
    FILE *fp;
    FILE *output;
    char *save_Backup = "iptables-save";
    char result[8192] = "";
    char buffer[128];



    
    
    dialog = gtk_file_chooser_dialog_new ("Save File",
                                        window,
                                        action,
                                        ("_Cancel"),
                                        GTK_RESPONSE_CANCEL,
                                        ("_Save"),
                                        GTK_RESPONSE_ACCEPT,
                                        NULL);
    chooser = GTK_FILE_CHOOSER (dialog);

    gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);

    
    gtk_file_chooser_set_current_name (chooser,
                                         ("Untitled document"));
    
    res = gtk_dialog_run (GTK_DIALOG (dialog));
    if (res == GTK_RESPONSE_ACCEPT){
        char *filename;

        filename = gtk_file_chooser_get_filename (chooser);
        output = popen(save_Backup,"r");
        if (!output) {
            return "popen failed!";
        }

        // read till end of process:
        while (!feof(output)) {

            // use buffer to read and add to result
            if (fgets(buffer, 128, output) != NULL){
                strcat(result, buffer);
            }
        }
        fclose(output);
        // printf("%s", result);
        fp = fopen(filename, "w");
        fprintf(fp, result);
        fclose(fp);
        g_free (filename);
    }

    gtk_widget_destroy (dialog);
}

void gtkToolBarRestore_clicked(GtkWidget *widget, gpointer data, GtkWindow *window){
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;
    char restore_Backup[256] = "iptables-restore < ";

    dialog = gtk_file_chooser_dialog_new ("Open File",
                                        window,
                                        action,
                                        ("_Cancel"),
                                        GTK_RESPONSE_CANCEL,
                                        ("_Open"),
                                        GTK_RESPONSE_ACCEPT,
                                        NULL);

    res = gtk_dialog_run (GTK_DIALOG (dialog));
    if (res == GTK_RESPONSE_ACCEPT){
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
        filename = gtk_file_chooser_get_filename (chooser);
        strcat(restore_Backup, filename);
        system(restore_Backup);
        
        g_free (filename);
    }
    gtk_widget_destroy (dialog);
}


void gtkToolBarQuit_clicked(GtkWidget *widget, gpointer data){
    
    gtk_main_quit();
}
int fileexists(const char * filename){
    /* try to open file to read */
    FILE *file;
    if (file = fopen(filename, "r")){
        fclose(file);
        return 1;
    }
    return 0;
}


int main (int   argc, char *argv[]){
    gchar *text;
    gint i;
    GtkWidget *row;
    GtkBuilder *builder;
    GObject *gtkToolbar;
    GObject *gtkToolBarSelect;
    GObject *gtkToolBarBackup;
    GObject *gtkToolBarRestore;
    GObject *gtkToolBarQuit;
    GObject *gtkListBox;
    GObject *gtkLabelLogo;
    GObject *gtkGrid;
    GError *error = NULL;
   
    gdk_threads_init ();
    gdk_threads_enter ();

    gtk_init (&argc, &argv);
    argc = argc;
    argv = argv;
    /* Construct a GtkBuilder instance and load our UI description */
    builder = gtk_builder_new ();
    if (gtk_builder_add_from_file (builder, "starter.ui", &error) == 0){
        g_printerr ("Error loading file: %s\n", error->message);
        g_clear_error (&error);
        return 1;
    }

    /* Connect signal handlers to the constructed widgets. */
    gtkWindowStarter = gtk_builder_get_object (builder, "starter");
    gtkGrid =  gtk_builder_get_object(builder, "gtkGrid");
    gtkToolBarSelect = gtk_builder_get_object(builder, "gtkToolBarSelect");
    gtkToolBarBackup = gtk_builder_get_object(builder, "gtkToolBarBackup");
    gtkToolBarRestore = gtk_builder_get_object(builder, "gtkToolBarRestore");
    gtkToolBarQuit = gtk_builder_get_object(builder, "gtkToolBarQuit");
    view = create_view_and_model ();
    
    gtk_grid_attach ((GtkGrid *)gtkGrid, view, 0, 3, 2, 1);

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));

    gtk_tree_selection_set_select_function(selection, view_selection_func, NULL, NULL);
    
    g_signal_connect (gtkWindowStarter, "destroy", G_CALLBACK (gtk_main_quit), NULL);
    g_signal_connect (gtkToolBarSelect, "clicked", gtkToolBarSelect_clicked, NULL);
    g_signal_connect (gtkToolBarBackup, "clicked", gtkToolBarBackup_clicked, gtkWindowStarter);
    g_signal_connect (gtkToolBarRestore, "clicked", gtkToolBarRestore_clicked, gtkWindowStarter);
    g_signal_connect (gtkToolBarQuit, "clicked", gtkToolBarQuit_clicked, NULL);
    
    
    
    gtk_widget_show_all (gtkWindowStarter);
    gtk_main ();
            
    gdk_threads_leave ();
    
    return 0;
}
