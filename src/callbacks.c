#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include "figure/figure.h"
#include "click_handle.h"
#include "draw/draw.h"
#include "binding/binding.h"
#include "callbacks.h"
#include "color/color.h"
#include "fd_format/fd_format.h"
#include "dbg.h"
#include "error/error.h"
#include "help/help.h"
#include "text/text.h"
#include "multi_obj/multi_obj.h"

extern list *figure_list;
extern GtkWidget *window;
double curs_x = 0, curs_y = 0,
	   click_x = 0, click_y = 0;
int direction_val = 1, type_val = 1;

static GtkWidget *target_window, *dialog;

gboolean draw_area_draw(GtkWidget *area, cairo_t *cr, gpointer data) {
	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_paint(cr);

	// draw figures
	dl_set_cairo_context(cr);
	list_crawl(figure_list, mol_draw_obj_from_node);
	dl_draw_preview();

	// draw cursor preview
	if (ch_get_draw_mode() != FG_TYPE_NONE) {
		cl_set_color(cr, CL_DEF_CURS_COLOR);
		cairo_arc(cr, curs_x, curs_y, 3, 0, 2 * G_PI);
		cairo_fill(cr);
	}

	return TRUE;
}

gboolean mouse_move(GtkWidget *widget, GdkEvent *event, GtkWidget *crd_label) {
	GdkEventMotion *em = (GdkEventMotion*)event;
	char crd[256];

	if (event->type == GDK_MOTION_NOTIFY) {
		curs_x = (double)em->x;
		curs_y = (double)em->y;

		bl_bind(figure_list, &curs_x, &curs_y);
		dl_set_preview_coords(curs_x, curs_y);

		sprintf(crd, "(%g;%g)", curs_x, curs_y);
		gtk_label_set_text(GTK_LABEL(crd_label), crd);

		gtk_widget_queue_draw(widget);
	}

	return TRUE;
}

gboolean mouse_click(GtkWidget *widget, GdkEvent *event, gpointer data) {
	GdkEventButton *eb = (GdkEventButton*)event;

	if (event->type == GDK_BUTTON_PRESS) {
		switch (eb->button) {
			case GDK_BUTTON_PRIMARY:
				click_x = (double)eb->x;
				click_y = (double)eb->y;

				bl_bind(figure_list, &click_x, &click_y);
				ch_click_handler(widget, figure_list, click_x, click_y);
			break;

			case GDK_BUTTON_SECONDARY:
				if (dl_get_preview()) {
					dl_switch_show_preview();
					ch_set_state(0);
				}
				else
					ch_unselect_last();
			
				gtk_widget_queue_draw(widget);
			break;
		}
	}

	return TRUE;
}

gboolean key_press(GtkWidget *widget, GdkEvent *event, gpointer data) {
	GdkEventKey *ek = (GdkEventKey*)event;

	switch (ek->keyval) {
		case GDK_KEY_Escape:
			
		break;
	}

	return TRUE;
}

void point_bttn_click(GtkWidget *bttn, gpointer data) {
	ch_set_draw_mode(FG_TYPE_POINT);
	hl_set_help(HC_POINT);
}

void line_bttn_click(GtkWidget *bttn, gpointer data) {
	ch_set_draw_mode(FG_TYPE_LINE_PP);
	hl_set_help(HC_START_POINT);
}

GtkWidget *lenght_entry, *angle_entry;

void line_la_bttn_click(GtkWidget *bttn, GtkWidget *parent_window) {
	GtkWidget *dialog_content;
	GtkWidget *lenght_label, *angle_label, *direction_label,
			  *ok_bttn, *apply_bttn, *direction_bttn;
	GtkWidget *lenght_box, *angle_box, *bttn_box, *direction_box, *main_box;

	dialog = gtk_dialog_new_with_buttons("Line (PRMT)", GTK_WINDOW(parent_window), (GtkDialogFlags)NULL, NULL, GTK_RESPONSE_NONE, NULL);
	dialog_content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
	g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_widget_destroy), dialog);

	// init labels
	lenght_label = gtk_label_new("Lenght");
	angle_label = gtk_label_new("Angle");

	// init entrys
	lenght_entry = gtk_entry_new();
	angle_entry = gtk_entry_new();

	// init direction widgets
	direction_label = gtk_label_new("Direction");
	direction_bttn = gtk_button_new_with_label("↑");

	g_signal_connect(G_OBJECT(direction_bttn), "clicked", G_CALLBACK(direction_bttn_click), NULL);

	// init buttons
	ok_bttn = gtk_button_new_with_label("OK");
	apply_bttn = gtk_button_new_with_label("Help");

	g_signal_connect(G_OBJECT(ok_bttn), "clicked", G_CALLBACK(line_la_dialog_ok_bttn_click), NULL);

	// init lenght- angle- boxes
	lenght_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	angle_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

	// pack boxes
	gtk_box_pack_start(GTK_BOX(lenght_box), lenght_label, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(lenght_box), lenght_entry, FALSE, FALSE, 5);

	gtk_box_pack_start(GTK_BOX(angle_box), angle_label, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(angle_box), angle_entry, FALSE, FALSE, 5);

	// init direction box
	direction_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

	// pack direction box
	gtk_box_pack_start(GTK_BOX(direction_box), direction_label, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(direction_box), direction_bttn, FALSE, FALSE, 5);

	// init button box
	bttn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

	// pack button box
	gtk_box_pack_start(GTK_BOX(bttn_box), ok_bttn, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(bttn_box), apply_bttn, TRUE, TRUE, 5);

	// init main box
	main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

	// pack box
	gtk_box_pack_start(GTK_BOX(main_box), lenght_box, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(main_box), angle_box, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(main_box), direction_box, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(main_box), bttn_box, TRUE, TRUE, 5);

	gtk_container_add(GTK_CONTAINER(dialog_content), main_box);
	gtk_widget_show_all(dialog);
}

void line_la_dialog_ok_bttn_click(GtkWidget *bttn, gpointer data) {
	static figure line_data;

	line_data.a1 = atof(gtk_entry_get_text(GTK_ENTRY(lenght_entry)));
	line_data.a2 = atof(gtk_entry_get_text(GTK_ENTRY(angle_entry))) * direction_val;

	ch_set_external_figure(&line_data);
	ch_set_draw_mode(FG_TYPE_LINE_LA);

	gtk_widget_destroy(dialog);

	hl_set_help(HC_POINT);
}

void rect_pp_bttn_click(GtkWidget *bttn, gpointer data) {
	ch_set_draw_mode(FG_TYPE_RECT_PP);
	hl_set_help(HC_START_POINT);
}

void set_lay_bttn_click(GtkWidget *bttn, GtkWidget *entry) {
	int new_lay = atoi(gtk_entry_get_text(GTK_ENTRY(entry)));

	figure_set_current_lay(new_lay);
}

void all_bttn_click(GtkWidget *bttn, GtkWidget *entry) {
	dl_switch_display_all_lays();

	gtk_entry_set_text(GTK_ENTRY(entry), "all");
} 

void set_window(GtkWidget *new_window) {
	target_window = new_window;
}

void save_bttn_click(GtkWidget *bttn, GtkWidget *parent_window) {
	GtkWidget *dialog, *dialog_content;
	GtkWidget *main_box;

	GtkWidget *file_name_entry, *ok_bttn;

	dialog = gtk_dialog_new_with_buttons("Save", GTK_WINDOW(parent_window), (GtkDialogFlags)NULL, NULL, GTK_RESPONSE_NONE, NULL);
	dialog_content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
	g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_widget_destroy), dialog);

	// init widgets
	file_name_entry = gtk_entry_new();
	ok_bttn = gtk_button_new_with_label("OK");

	g_signal_connect(G_OBJECT(ok_bttn), "clicked", G_CALLBACK(save_dialog_ok_bttn_click), file_name_entry);

	// init main_box
	main_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_box_pack_start(GTK_BOX(main_box), file_name_entry, TRUE, TRUE, 3);
	gtk_box_pack_start(GTK_BOX(main_box), ok_bttn, FALSE, FALSE, 3);

	gtk_container_add(GTK_CONTAINER(dialog_content), main_box);
	gtk_widget_show_all(dialog);
}

void save_dialog_ok_bttn_click(GtkWidget *bttn, GtkWidget *entry) {
	const char *name = gtk_entry_get_text(GTK_ENTRY(entry));

	fdl_target_file((char*)name);
	fdl_write_from_list(figure_list);
}

void open_bttn_click(GtkWidget *bttn, GtkWidget *parent_window) {
	GtkWidget *dialog, *dialog_content;
	GtkWidget *main_box;

	GtkWidget *file_name_entry, *ok_bttn;

	dialog = gtk_dialog_new_with_buttons("Open", GTK_WINDOW(parent_window), (GtkDialogFlags)NULL, NULL, GTK_RESPONSE_NONE, NULL);
	dialog_content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
	g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_widget_destroy), dialog);

	// init widgets
	file_name_entry = gtk_entry_new();
	ok_bttn = gtk_button_new_with_label("OK");

	g_signal_connect(G_OBJECT(ok_bttn), "clicked", G_CALLBACK(open_dialog_ok_bttn_click), file_name_entry);

	// init main_box
	main_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_box_pack_start(GTK_BOX(main_box), file_name_entry, TRUE, TRUE, 3);
	gtk_box_pack_start(GTK_BOX(main_box), ok_bttn, FALSE, FALSE, 3);

	gtk_container_add(GTK_CONTAINER(dialog_content), main_box);
	gtk_widget_show_all(dialog);
}

void open_dialog_ok_bttn_click(GtkWidget *bttn, GtkWidget *entry) {
	const char *name = gtk_entry_get_text(GTK_ENTRY(entry));

	fdl_target_file((char*)name);
	fdl_read_file(figure_list);
}

GtkWidget *width_entry, *height_entry, *direction_bttn;

void rect_wh_bttn_click(GtkWidget *bttn, GtkWidget *parent_window) {
	GtkWidget *dialog_content;
	GtkWidget *width_label, *height_label, *direction_label,
			  *ok_bttn, *apply_bttn;
	GtkWidget *width_box, *height_box, *direction_box, *bttn_box, *main_box;

	dialog = gtk_dialog_new_with_buttons("Rect (PRMT)", GTK_WINDOW(parent_window), (GtkDialogFlags)NULL, NULL, GTK_RESPONSE_NONE, NULL);
	dialog_content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
	g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_widget_destroy), dialog);

	// init labels
	width_label = gtk_label_new("Width");
	height_label = gtk_label_new("Height");

	// init entrys
	width_entry = gtk_entry_new();
	height_entry = gtk_entry_new();

	// init direction widgets
	direction_label = gtk_label_new("Direction");
	direction_bttn = gtk_button_new_with_label("↑");

	g_signal_connect(G_OBJECT(direction_bttn), "clicked", G_CALLBACK(direction_bttn_click), NULL);

	// init buttons
	ok_bttn = gtk_button_new_with_label("OK");
	apply_bttn = gtk_button_new_with_label("Help");

	g_signal_connect(G_OBJECT(ok_bttn), "clicked", G_CALLBACK(rect_wh_dialog_ok_bttn_click), NULL);

	// init lenght- angle- boxes
	width_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	height_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

	// pack boxes
	gtk_box_pack_start(GTK_BOX(width_box), width_label, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(width_box), width_entry, FALSE, FALSE, 5);

	gtk_box_pack_start(GTK_BOX(height_box), height_label, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(height_box), height_entry, FALSE, FALSE, 5);

	// init direction box
	direction_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

	// pack direction box
	gtk_box_pack_start(GTK_BOX(direction_box), direction_label, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(direction_box), direction_bttn, FALSE, FALSE, 5);

	// init button box
	bttn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

	// pack button box
	gtk_box_pack_start(GTK_BOX(bttn_box), ok_bttn, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(bttn_box), apply_bttn, TRUE, TRUE, 5);

	// init main box
	main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

	// pack box
	gtk_box_pack_start(GTK_BOX(main_box), width_box, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(main_box), height_box, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(main_box), direction_box, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(main_box), bttn_box, TRUE, TRUE, 5);

	gtk_container_add(GTK_CONTAINER(dialog_content), main_box);
	gtk_widget_show_all(dialog);
}

void direction_bttn_click(GtkWidget *bttn, gpointer data) {
	const char *text = NULL;

	text = gtk_button_get_label(GTK_BUTTON(bttn));
	if (!strcmp(text, "↑")) {
		gtk_button_set_label(GTK_BUTTON(bttn), "↓");
		direction_val = -1;
	}
	else {
		gtk_button_set_label(GTK_BUTTON(bttn), "↑");
		direction_val = 1;
	}
}

void rect_wh_dialog_ok_bttn_click(GtkWidget *bttn, gpointer data) {
	static figure rect_data;

	rect_data.a1 = atof(gtk_entry_get_text(GTK_ENTRY(width_entry)));
	rect_data.a2 = atof(gtk_entry_get_text(GTK_ENTRY(height_entry))) * direction_val;

	ch_set_external_figure(&rect_data);
	ch_set_draw_mode(FG_TYPE_RECT_WH);

	gtk_widget_destroy(dialog);

	hl_set_help(HC_POINT);
}

void add_projection_lay_bttn_click(GtkWidget *bttn, GtkWidget *entry) {
	char *text = (char*)gtk_entry_get_text(GTK_ENTRY(entry));
	int is_prj_lay = 0, entry_lay = 0;

	entry_lay = atoi(text);

	is_prj_lay = figure_is_projection_lay_list(figure_list, entry_lay);
	if (is_prj_lay)
		figure_set_visible_by_lay_list(figure_list, entry_lay, VM_HIDE);
	else
		figure_set_visible_by_lay_list(figure_list, entry_lay, VM_PROJECTION);
}

void help_bttn_click(GtkWidget *bttn, GtkWidget *parent_window) {
	GtkWidget *help_dialog, *dialog_content;
	GtkWidget *main_box;
	GtkWidget *help_label, *logo;

	help_dialog = gtk_dialog_new_with_buttons("Help", GTK_WINDOW(parent_window), (GtkDialogFlags)NULL, NULL, GTK_RESPONSE_NONE, NULL);
	dialog_content = gtk_dialog_get_content_area(GTK_DIALOG(help_dialog));
	g_signal_connect_swapped(help_dialog, "response", G_CALLBACK(gtk_widget_destroy), help_dialog);

	// widgets label
	help_label = gtk_label_new(hl_get_help(HC_MAIN));
	logo = gtk_image_new_from_file("res/fd.png");

	gtk_label_set_selectable(GTK_LABEL(help_label), TRUE);

	// init main box
	main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	gtk_box_pack_start(GTK_BOX(main_box), logo, TRUE, TRUE, 10);
	gtk_box_pack_start(GTK_BOX(main_box), help_label, TRUE, TRUE, 10);

	gtk_container_add(GTK_CONTAINER(dialog_content), main_box);
	gtk_widget_show_all(help_dialog);
}

void circle_bttn_click(GtkWidget *bttn, gpointer data) {
	ch_set_draw_mode(FG_TYPE_CIRCLE);

	hl_set_help(HC_CENTER_POINT);
}

void arc_bttn_click(GtkWidget *bttn, GtkWidget *parent_window) {
	ch_set_draw_mode(FG_TYPE_ARC);
}

void curs_bttn_click(GtkWidget *bttn, gpointer data) {
	ch_set_draw_mode(FG_TYPE_NONE);
	hl_set_help(HC_CUROR);
}

void unselect(list *node) {
	figure *fptr = (figure*)node->data;

	if (fptr && fptr->visible == VM_SELECTED)
		fptr->visible = VM_SHOW;
}

void options_bttn_click(GtkWidget *bttn, GtkWidget *parent_window) {
	GtkWidget *dialog_content;
	GtkWidget *dialog_box;

	dialog = gtk_dialog_new_with_buttons("Options", GTK_WINDOW(parent_window), (GtkDialogFlags)NULL, NULL, GTK_RESPONSE_NONE, NULL);
	dialog_content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
	g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_widget_destroy), dialog);

	// init main box
	dialog_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

	gtk_container_add(GTK_CONTAINER(dialog_content), dialog_box);
	gtk_widget_show_all(dialog);
}

void del_bttn_click(GtkWidget *bttn, GtkWidget *da) {
	list *node, *node_next, *node_prev;
	figure *fptr;

	node = figure_list;
	while (node) {
		//fptr = (figure*)node->data;
		fptr = mol_conv_to_figure(node);
		if (!fptr) {
			node = node->next;

			continue;
		}

		if (fptr->visible == VM_SELECTED) {
			node_prev = node->prev;
			node_next = node->next;

			if (node_prev)
				node_prev->next = node_next;
			if (node_next)
				node_next->prev = node_prev;

			figure_free(fptr);
			list_free_node(node);

			node = node_next;

			continue;
		}

		node = node->next;
	}

#ifdef DBG
	list_crawl(figure_list, list_dump_node);
	putc('\n', stdout);
#endif

	gtk_widget_queue_draw(da);
}

void prm_bttn_click(GtkWidget *bttn, GtkWidget *elms[]) {
	static int mode = 0;

	if (!mode)
		gtk_button_set_label(GTK_BUTTON(bttn), "prmt");
	else
		gtk_button_set_label(GTK_BUTTON(bttn), "free");

	for (int i = 0; i < DRAW_BUTTONS; i++) {
		if (!mode) {
			if (strstr((char*)gtk_button_get_label(GTK_BUTTON(elms[i])), "free"))
				gtk_widget_hide(elms[i]);
			else
				gtk_widget_show(elms[i]);
		}
		else {
			if (strstr((char*)gtk_button_get_label(GTK_BUTTON(elms[i])), "prmt"))
				gtk_widget_hide(elms[i]);
			else
				gtk_widget_show(elms[i]);
		}
	}

	mode = !mode;
}

void move_bttn_click(GtkWidget *bttn, gpointer data) {
	ch_set_draw_mode(WM_MOVE);
	hl_set_help(HC_BASE_POINT);
}

void cp_bttn_click(GtkWidget *bttn, gpointer data) {
	ch_set_draw_mode(WM_CP);
	hl_set_help(HC_BASE_POINT);
}

void dc_bttn_click(GtkWidget *bttn, GtkWidget *draw_area) {
	list_crawl(figure_list, cb_dc);

	gtk_widget_queue_draw(draw_area);
}

void cb_dc(list *lptr) {
	figure *fptr = NULL, *elms = NULL, *new = NULL;
	list *last = NULL;

	fptr = figure_get_from_node(lptr);
	if (!fptr)
		return ;

	if (fptr->visible == VM_SELECTED) {
		if (fptr->type == FG_TYPE_RECT_PP) {
			elms = figure_rect_decompose(fptr);

			for (int i = 0; i < 4; i++) {
				list_add_node(lptr);

				last = list_get_last(lptr);

				new = figure_new_point(0, 0);
				figure_copy(new, &elms[i]);
				new->visible = VM_SHOW;

				list_set_data(last, new);
			}

			figure_free(fptr);
			list_set_data(lptr, NULL);
		}
		else {
			el_call_error(ET_WRONG_SELECTING);
		}
	}
}

void rot_bttn_click(GtkWidget *bttn, GtkWidget *parent_window) {
	GtkWidget *dialog_content;
	GtkWidget *dialog_box;
	GtkWidget *info_label, *angle_entry, *apply_bttn;

	dialog = gtk_dialog_new_with_buttons("Rotate", GTK_WINDOW(parent_window), (GtkDialogFlags)NULL, NULL, GTK_RESPONSE_NONE, NULL);
	dialog_content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
	g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_widget_destroy), dialog);

	// init widgets
	info_label = gtk_label_new("Specify the angle of rotation (in degrees)");
	angle_entry = gtk_entry_new();
	apply_bttn = gtk_button_new_with_label("Apply");

	g_signal_connect(G_OBJECT(apply_bttn), "clicked", G_CALLBACK(rot_dialog_apply_bttn), angle_entry);

	// init main box
	dialog_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

	gtk_box_pack_start(GTK_BOX(dialog_box), info_label, FALSE, FALSE, 10);
	gtk_box_pack_start(GTK_BOX(dialog_box), angle_entry, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(dialog_box), apply_bttn, FALSE, FALSE, 5);

	gtk_container_add(GTK_CONTAINER(dialog_content), dialog_box);
	gtk_widget_show_all(dialog);
}

void rot_dialog_apply_bttn(GtkWidget *bttn, GtkWidget *entry) {
	static figure ang_figure;

	ang_figure.x = atof(gtk_entry_get_text(GTK_ENTRY(entry)));
	ch_set_external_figure(&ang_figure);

	gtk_widget_destroy(dialog);

	ch_set_draw_mode(WM_ROTATE);

	hl_set_help(HC_BASE_POINT);
}

void circle_prm_bttn_click(GtkWidget *bttn, GtkWidget *parent_window) {
	GtkWidget *dialog_content;
	GtkWidget *radii_label, *radii_entry, *ok_bttn;
	GtkWidget *radii_box, *bttn_box, *main_box;

	dialog = gtk_dialog_new_with_buttons("Circle (PRMT)", GTK_WINDOW(parent_window), (GtkDialogFlags)NULL, NULL, GTK_RESPONSE_NONE, NULL);
	dialog_content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
	g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_widget_destroy), dialog);

	// init labels
	radii_label = gtk_label_new("Circle Radius");

	// init entrys
	radii_entry = gtk_entry_new();

	// init buttons
	ok_bttn = gtk_button_new_with_label("OK");

	g_signal_connect(G_OBJECT(ok_bttn), "clicked", G_CALLBACK(circle_dialog_ok_bttn_click), radii_entry);

	// init lenght- angle- boxes
	radii_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

	// pack boxes
	gtk_box_pack_start(GTK_BOX(radii_box), radii_label, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(radii_box), radii_entry, FALSE, FALSE, 5);

	// init button box
	bttn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

	// pack button box
	gtk_box_pack_start(GTK_BOX(bttn_box), ok_bttn, TRUE, TRUE, 5);

	// init main box
	main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

	// pack box
	gtk_box_pack_start(GTK_BOX(main_box), radii_box, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(main_box), bttn_box, TRUE, TRUE, 5);

	gtk_container_add(GTK_CONTAINER(dialog_content), main_box);
	gtk_widget_show_all(dialog);
}

void circle_dialog_ok_bttn_click(GtkWidget *bttn, GtkWidget *entry) {
	static figure circle_data;

	circle_data.a1 = atof(gtk_entry_get_text(GTK_ENTRY(entry)));

	ch_set_external_figure(&circle_data);
	ch_set_draw_mode(FG_TYPE_CIRCLE_PRM);

	hl_set_help(HC_CENTER_POINT);

	gtk_widget_destroy(dialog);
}

GtkWidget *ang1_entry, *ang2_entry, *radii_entry;

void arc_prm_bttn_click(GtkWidget *bttn, GtkWidget *parent_window) {
	GtkWidget *dialog_content;
	GtkWidget *ang1_label, *ang2_label,
			  *radii_label,
			  *ok_bttn;
	GtkWidget *entrys[3];
	GtkWidget *ang1_box, *ang2_box, *radii_box, *bttn_box, *main_box;

	dialog = gtk_dialog_new_with_buttons("Arc (PRMT)", GTK_WINDOW(parent_window), (GtkDialogFlags)NULL, NULL, GTK_RESPONSE_NONE, NULL);
	dialog_content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
	g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_widget_destroy), dialog);

	// init labels
	ang1_label = gtk_label_new("First angle");
	ang2_label = gtk_label_new("Second angle");
	radii_label = gtk_label_new("Arc radius");

	// init entrys
	ang1_entry = gtk_entry_new();
	ang2_entry = gtk_entry_new();
	radii_entry = gtk_entry_new();

	// init buttons
	ok_bttn = gtk_button_new_with_label("OK");

	entrys[0] = radii_entry;
	entrys[1] = ang1_entry;
	entrys[2] = ang2_entry;
	g_signal_connect(G_OBJECT(ok_bttn), "clicked", G_CALLBACK(arc_dialog_ok_bttn_click), entrys);

	// init lenght- angle- boxes
	ang1_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	ang2_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	radii_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

	// pack boxes
	gtk_box_pack_start(GTK_BOX(ang1_box), ang1_label, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(ang1_box), ang1_entry, TRUE, TRUE, 5);

	gtk_box_pack_start(GTK_BOX(ang2_box), ang2_label, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(ang2_box), ang2_entry, TRUE, TRUE, 5);

	gtk_box_pack_start(GTK_BOX(radii_box), radii_label, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(radii_box), radii_entry, TRUE, TRUE, 5);

	// init button box
	bttn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

	// pack button box
	gtk_box_pack_start(GTK_BOX(bttn_box), ok_bttn, TRUE, TRUE, 5);

	// init main box
	main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

	// pack box
	gtk_box_pack_start(GTK_BOX(main_box), ang1_box, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(main_box), ang2_box, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(main_box), radii_box, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(main_box), bttn_box, TRUE, TRUE, 5);

	gtk_container_add(GTK_CONTAINER(dialog_content), main_box);
	gtk_widget_show_all(dialog);
}

void arc_dialog_ok_bttn_click(GtkWidget *bttn, gpointer data) {
	static figure arc_data;

	// radius
	arc_data.a1 = atof(gtk_entry_get_text(GTK_ENTRY(radii_entry)));

	// angles
	arc_data.a2 = atof(gtk_entry_get_text(GTK_ENTRY(ang1_entry)));
	arc_data.a3 = atof(gtk_entry_get_text(GTK_ENTRY(ang2_entry)));

	ch_set_external_figure(&arc_data);
	ch_set_draw_mode(FG_TYPE_ARC_PRM);

	hl_set_help(HC_CENTER_POINT);

	gtk_widget_destroy(dialog);
}

GtkWidget *text_size_entry, *text_font_entry, *text_color_entry;

void text_bttn_click(GtkWidget *bttn, GtkWindow *parent_window) {
	GtkWidget *dialog_content;
	GtkWidget *text_view, *ok_bttn, *help_bttn;
	GtkWidget *text_size_label, *text_font_label, *text_color_label;
	GtkWidget *bttn_box, *label_size_box, *label_font_box, *label_color_box, *label_opt_box, *main_box;
	GtkTextBuffer *text_buffer;

	dialog = gtk_dialog_new_with_buttons("Text", GTK_WINDOW(parent_window), (GtkDialogFlags)NULL, NULL, GTK_RESPONSE_NONE, NULL);
	dialog_content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
	g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_widget_destroy), dialog);

	// label options
	text_size_label = gtk_label_new("Text size");
	text_font_label = gtk_label_new("Font");
	text_color_label = gtk_label_new("Color (r g b)");

	text_size_entry = gtk_entry_new();
	text_font_entry = gtk_entry_new();
	text_color_entry = gtk_entry_new();

	// create text buffer & view
	text_buffer = gtk_text_buffer_new(NULL);
	text_view = gtk_text_view_new_with_buffer(text_buffer);

	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD_CHAR);

	// buttons
	ok_bttn = gtk_button_new_with_label("OK");
	help_bttn = gtk_button_new_with_label("Help");

	g_signal_connect(G_OBJECT(ok_bttn), "clicked", G_CALLBACK(text_dialog_ok_bttn_click), text_buffer);

	// pack label size box
	label_size_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

	gtk_box_pack_start(GTK_BOX(label_size_box), text_size_label, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(label_size_box), text_size_entry, TRUE, TRUE, 5);

	// pack label font box
	label_font_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

	gtk_box_pack_start(GTK_BOX(label_font_box), text_font_label, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(label_font_box), text_font_entry, TRUE, TRUE, 5);

	// pack label color box
	label_color_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

	gtk_box_pack_start(GTK_BOX(label_color_box), text_color_label, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(label_color_box), text_color_entry, TRUE, TRUE, 5);

	// pack label options box
	label_opt_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

	gtk_box_pack_start(GTK_BOX(label_opt_box), label_size_box, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(label_opt_box), label_font_box, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(label_opt_box), label_color_box, FALSE, FALSE, 5);

	// pack button box
	bttn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

	gtk_box_pack_start(GTK_BOX(bttn_box), ok_bttn, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(bttn_box), help_bttn, TRUE, TRUE, 5);

	// pack main box
	main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

	gtk_box_pack_start(GTK_BOX(main_box), label_opt_box, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(main_box), text_view, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(main_box), bttn_box, FALSE, FALSE, 5);

	gtk_container_add(GTK_CONTAINER(dialog_content), main_box);
	gtk_widget_show_all(dialog);
}

void text_dialog_ok_bttn_click(GtkWidget *bttn, GtkTextBuffer *tb) {
	GtkTextIter start, end;
	char *buf = NULL, *font = NULL, col_str[256] = "", *col_token = NULL;
	int size = 0, color[3] = {0, 0, 0}, i = 0;
	list *last;
	text *tptr;

	gtk_text_buffer_get_start_iter(tb, &start);
	gtk_text_buffer_get_end_iter(tb, &end);

	buf = gtk_text_buffer_get_text(tb, &start, &end, TRUE);
	font = (char*)gtk_entry_get_text(GTK_ENTRY(text_font_entry));
	size = atoi(gtk_entry_get_text(GTK_ENTRY(text_size_entry)));

	strcpy(col_str, (char*)gtk_entry_get_text(GTK_ENTRY(text_color_entry)));
	col_token = strtok(col_str, " ");
	while (col_token) {
		color[i++] = atoi(col_token);

		col_token = strtok(NULL, " ");
	}

	list_add_node(figure_list);
	last = list_get_last(figure_list);
	last->dt = OT_TEXT;

	tptr = tl_new(font, size, color[0], color[1], color[2]);
	tl_add_buffer(tptr, buf);

	tptr->visible = VM_NOT_FINISHED;

	list_set_data(last, tptr);

	gtk_widget_destroy(dialog);

	ch_set_draw_mode(WM_TEXT);
}
