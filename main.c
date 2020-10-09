/*
 * main.c -- main fd file, contains gui
 * v0.1
 * 06.10.2020
 * by Centrix
 */

#include <stdio.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "data/list.h"
#include "figure.h"

list *figure_list = NULL;

int main() {
	GtkWidget *window;

	GtkWidget *main_box, *right_box, *draw_box, *down_tool_box;

	GtkWidget *scrolled_window, *draw_area;
	GtkWidget *point_bttn, *line_pp_bttn, *line_la_bttn, *rect_pp_bttn, *rect_wh_bttn, *arc_tp_bttn, *circle_rc_bttn, *help_bttn;
	GtkWidget *lay_entry, *set_bttn, *all_bttn, *add_projection_lay_bttn, *crd_label, *hint_label;
	GtkWidget *save_bttn, *open_bttn, *ver_sep;

	figure_list = list_init_node(NULL);

	gtk_init(NULL, NULL);

	// init window
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "FlatDraw");
	gtk_window_set_default_size(GTK_WINDOW(window), 1000, 800);
	gtk_window_set_default_icon_from_file("fd.ico", NULL);

	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

	// init drawing area
	draw_area = gtk_drawing_area_new();
	gtk_widget_set_size_request(draw_area, 1000, 800);

	// init scrolled window
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_ALWAYS, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scrolled_window), draw_area);

	// init buttons
	point_bttn = gtk_button_new_with_label("Point");
	line_pp_bttn = gtk_button_new_with_label("Line (PP)");
	line_la_bttn = gtk_button_new_with_label("Line (LA)");
	rect_pp_bttn = gtk_button_new_with_label("Rect (PP)");
	rect_wh_bttn = gtk_button_new_with_label("Rect (WH)");
	arc_tp_bttn = gtk_button_new_with_label("Arc (3P)");
	circle_rc_bttn = gtk_button_new_with_label("Circle (RC)");
	help_bttn = gtk_button_new_with_label("Help");

	g_signal_connect(G_OBJECT(point_bttn), "clicked", G_CALLBACK(point_bttn_click), NULL);
	g_signal_connect(G_OBJECT(line_pp_bttn), "clicked", G_CALLBACK(line_bttn_click), NULL);
	g_signal_connect(G_OBJECT(line_la_bttn), "clicked", G_CALLBACK(line_la_bttn_click), window);
	g_signal_connect(G_OBJECT(rect_pp_bttn), "clicked", G_CALLBACK(rect_pp_bttn_click), NULL);
	g_signal_connect(G_OBJECT(rect_wh_bttn), "clicked", G_CALLBACK(rect_wh_bttn_click), window);
	g_signal_connect(G_OBJECT(circle_rc_bttn), "clicked", G_CALLBACK(circle_bttn_click), NULL);
	g_signal_connect(G_OBJECT(arc_tp_bttn), "clicked", G_CALLBACK(arc_bttn_click), window);

	g_signal_connect(G_OBJECT(help_bttn), "clicked", G_CALLBACK(help_bttn_click), window);

	// init down tool panel widgets
	lay_entry = gtk_entry_new();
	set_bttn = gtk_button_new_with_label("Set");
	all_bttn = gtk_button_new_with_label("All");
	add_projection_lay_bttn = gtk_button_new_with_label("+ / -");

	crd_label = gtk_label_new("(0; 0)");
	hint_label = gtk_label_new("Select tool");

	g_signal_connect(G_OBJECT(set_bttn), "clicked", G_CALLBACK(set_lay_bttn_click), lay_entry);
	g_signal_connect(G_OBJECT(all_bttn), "clicked", G_CALLBACK(all_bttn_click), lay_entry);
	g_signal_connect(G_OBJECT(add_projection_lay_bttn), "clicked", G_CALLBACK(add_projection_lay_bttn_click), lay_entry);

	// file operations widgets
	save_bttn = gtk_button_new_with_label("Save");
	open_bttn = gtk_button_new_with_label("Open");
	ver_sep = gtk_separator_new(GTK_ORIENTATION_VERTICAL);

	g_signal_connect(G_OBJECT(save_bttn), "clicked", G_CALLBACK(save_bttn_click), window);
	g_signal_connect(G_OBJECT(open_bttn), "clicked", G_CALLBACK(open_bttn_click), window);

	// init right box
	right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_pack_start(GTK_BOX(right_box), point_bttn, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(right_box), line_pp_bttn, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(right_box), line_la_bttn, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(right_box), rect_pp_bttn, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(right_box), rect_wh_bttn, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(right_box), arc_tp_bttn, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(right_box), circle_rc_bttn, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(right_box), help_bttn, TRUE, TRUE, 0);

	// init draw box
	draw_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_pack_start(GTK_BOX(draw_box), scrolled_window, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(draw_box), right_box, FALSE, FALSE, 0);

	// init down tool box
	down_tool_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_pack_start(GTK_BOX(down_tool_box), crd_label, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(down_tool_box), hint_label, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(down_tool_box), lay_entry, TRUE, TRUE, 3);
	gtk_box_pack_start(GTK_BOX(down_tool_box), set_bttn, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(down_tool_box), all_bttn, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(down_tool_box), add_projection_lay_bttn, FALSE, FALSE, 0);

	// file widgets
	gtk_box_pack_start(GTK_BOX(down_tool_box), ver_sep, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(down_tool_box), save_bttn, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(down_tool_box), open_bttn, FALSE, FALSE, 0);

	// init main_box
	main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_pack_start(GTK_BOX(main_box), draw_box, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(main_box), down_tool_box, FALSE, FALSE, 0);

	// add events
	g_signal_connect(G_OBJECT(draw_area), "draw", G_CALLBACK(draw_area_draw), NULL);
	g_signal_connect(G_OBJECT(draw_area), "motion-notify-event", G_CALLBACK(mouse_move), crd_label);
	g_signal_connect(G_OBJECT(draw_area), "button-press-event", G_CALLBACK(mouse_click), NULL);

	gtk_widget_add_events(draw_area, GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK);

	gtk_container_add(GTK_CONTAINER(window), main_box);
	gtk_widget_show_all(window);

	gtk_main();

	list_crawl(figure_list, figure_free_list);
	list_free_list(figure_list);

	return 0;
}
