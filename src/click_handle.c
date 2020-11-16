#include <stdio.h>
#include <math.h>
#include <gtk/gtk.h>

#include "data/list.h"
#include "figure.h"
#include "click_handle.h"
#include "geometry.h"
#include "draw.h"
#include "callbacks.h"

#include "dbg.h"

static int draw_mode = FG_TYPE_POINT, state = 0;
static figure *ext_figure,
			  tmp_figure;
figure *last_selected_figure;

void ch_set_draw_mode(int new_mode) {
	draw_mode = new_mode;
}

int ch_get_draw_mode() {
	return draw_mode;
}

void ch_set_external_figure(figure *fptr) {
	ext_figure = fptr;
}

void ch_set_state(int new_state) {
	state = new_state;
}

void ch_click_handler(GtkWidget *draw_area, list *lptr, int x, int y) {
	switch (draw_mode) {
		case FG_TYPE_POINT:
			ch_add_point(draw_area, lptr, x, y);
		break;

		case FG_TYPE_LINE_PP:
			ch_add_line_pp(draw_area, lptr, x, y);
		break;

		case FG_TYPE_LINE_LA:
			ch_add_line_la(draw_area, lptr, x, y);
		break;

		case FG_TYPE_RECT_PP:
			ch_add_rect_pp(draw_area, lptr, x, y);
		break;

		case FG_TYPE_RECT_WH:
			ch_add_rect_wh(draw_area, lptr, x, y);
		break;

		case FG_TYPE_CIRCLE:
			ch_add_circle(draw_area, lptr, x, y);
		break;

		case FG_TYPE_ARC:
			ch_add_arc(draw_area, lptr, x, y);
		break;

		case FG_TYPE_NONE:
			ch_click_cursor_select(draw_area, lptr, x, y);
		break;
	}
}

void ch_add_point(GtkWidget *draw_area, list *lptr, int x, int y) {
	list *last;
	figure *point;

	list_add_node(lptr);

	last = list_get_last(lptr);
	point = figure_new_point(x, y);

	list_set_data(last, point);

	gtk_widget_queue_draw(draw_area);
}

void ch_add_line_pp(GtkWidget *draw_area, list *lptr, int x, int y) {
	list *last;
	figure *line;

	if (!state) {
		figure_fill(&tmp_figure, x, y, 0, 0, FG_TYPE_LINE_PP);
		tmp_figure.visible = VM_PREVIEW;

		dl_send_preview_figure(&tmp_figure);
	}
	else {
		list_add_node(lptr);

		last = list_get_last(lptr);
		line = figure_new_line_pp(0, 0, 0, 0);

		figure_copy(line, &tmp_figure);

		line->a1 = x;
		line->a2 = y;

		line->visible = VM_SHOW;

		list_set_data(last, line);
	}
	
	state = !state;
	dl_switch_show_preview();
	gtk_widget_queue_draw(draw_area);
}

void ch_add_line_la(GtkWidget *draw_area, list *lptr, int x, int y) {
	list *last;
	figure *line;

	list_add_node(lptr);

	last = list_get_last(lptr);
	line = figure_new_line_pp(x, y, 0, 0);
	line->visible = VM_SHOW;

	gel_calculate_line_la(line, ext_figure->a1, ext_figure->a2);

	list_set_data(last, line);

	gtk_widget_queue_draw(draw_area);
}

void ch_add_rect_pp(GtkWidget *draw_area, list *lptr, int x, int y) {
	list *last;
	figure *rect;

	if (!state) {
		figure_fill(&tmp_figure, x, y, 0, 0, FG_TYPE_RECT_PP);
		tmp_figure.visible = VM_PREVIEW;

		dl_send_preview_figure(&tmp_figure);	
	}
	else {
		list_add_node(lptr);

		last = list_get_last(lptr);
		rect = figure_new_rect_pp(0, 0, 0, 0);

		figure_copy(rect, &tmp_figure);

		rect->a1 = x;
		rect->a2 = y;

		rect->visible = VM_SHOW;

		list_set_data(last, rect);
	}

	state = !state;
	dl_switch_show_preview();
	gtk_widget_queue_draw(draw_area);
}

void ch_add_rect_wh(GtkWidget *draw_area, list *lptr, int x, int y) {
	list *last;
	figure *rect;

	list_add_node(lptr);

	last = list_get_last(lptr);
	rect = figure_new_rect_pp(x, y, 0, 0);
	rect->visible = VM_SHOW;

	rect->a1 = rect->x + ext_figure->a1;
	rect->a2 = rect->y + ext_figure->a2;

	list_set_data(last, rect);

	gtk_widget_queue_draw(draw_area);
}

void ch_add_circle(GtkWidget *draw_area, list *lptr, int x, int y) {
	figure *circle, *rad, *center_point;
	list *last;
	double radii;

	if (!state) {
		figure_fill(&tmp_figure, x, y, 50, 0, FG_TYPE_CIRCLE);
		tmp_figure.visible = VM_PREVIEW;

		dl_send_preview_figure(&tmp_figure);
	}
	else {
		list_add_node(lptr);

		last = list_get_last(lptr);
		circle = figure_new_circle(0, 0, 0);

		// get readius
		rad = figure_new_line_pp(tmp_figure.x, tmp_figure.y, x, y);
		radii = gel_calculate_lenght(rad);

		figure_copy(circle, &tmp_figure);

		//set radii
		circle->a1 = radii;
		circle->visible = VM_SHOW;

		list_set_data(last, circle);

		// add center point
		list_add_node(lptr);

		last = list_get_last(lptr);
		center_point = figure_new_point(tmp_figure.x, tmp_figure.y);

		list_set_data(last, center_point);
	}

	state = !state;
	dl_switch_show_preview();
	gtk_widget_queue_draw(draw_area);
}

void ch_add_arc(GtkWidget *draw_area, list *lptr, double x, double y) {
	static double x1, y1, x2, y2, xh, yh, a_side, ang1, ang2;
	static int state = 0;
	figure l, cpoint, *arc, *arc_c, b_side_line, c_side_line, a_side_line;
	list *last;
	double R;

	if (!state) {
		x1 = x;
		y1 = y;

		state = 1;
	}
	else if (state == 1) {
		x2 = x;
		y2 = y;

		figure_fill(&l, x1, y1, x2, y2, FG_TYPE_LINE_PP);

		// get base of arc, and middle point of base
		a_side = gel_calculate_lenght(&l);
		printf("Arc: a = %g\n", a_side);

		state = 2;
	}
	else {
		xh = x;
		yh = y;
		printf("Arc: xh = %g, yh = %g\n", xh, yh);

		// calculate the a, b and c side of triangle
		figure_fill(&b_side_line, x1, y1, xh, yh, FG_TYPE_LINE_PP);
		//b_side = gel_calculate_lenght(&b_side_line);

		figure_fill(&c_side_line, xh, yh, x2, y2, FG_TYPE_LINE_PP);
		//c_side = gel_calculate_lenght(&c_side_line);

		figure_fill(&a_side_line, x1, y1, x2, y2, FG_TYPE_LINE_PP);

		// get center point
		cpoint = *gel_get_arc_center(&b_side_line, &c_side_line);
		//mpoint = *gel_get_middle_point(&a_side_line);

		// calculate R
		figure_fill(&l, cpoint.x, cpoint.y, xh, yh, FG_TYPE_LINE_PP);
		R = gel_calculate_lenght(&l);

		// get angel of first point
		figure_fill(&l, cpoint.x, cpoint.y, x1, y1, FG_TYPE_LINE_PP);
		ang1 = gel_calculate_line_angle(&l);

		// get angel of second point
		figure_fill(&l, cpoint.x, cpoint.y, x2, y2, FG_TYPE_LINE_PP);
		ang2 = gel_calculate_line_angle(&l);

		printf("Arc: C(%g; %g), a1 = %g, a2 = %g, R = %g\n", cpoint.x, cpoint.y, ang1, ang2, R);

		// make arc
		list_add_node(lptr);

		last = list_get_last(lptr);
		arc = figure_new_arc(cpoint.x, cpoint.y, R, ang1, ang2);
		arc->visible = VM_SHOW;

		list_set_data(last, arc);

		state = 0;

		// make center point
		list_add_node(lptr);

		last = list_get_last(lptr);
		arc_c = figure_new_point(cpoint.x, cpoint.y);

		list_set_data(last, arc_c);
		

		gtk_widget_queue_draw(draw_area);
	}
}

void ch_click_cursor_select(GtkWidget *draw_area, list *lptr, double x, double y) {
	figure *fptr, *curs;
	list *node;
	int end = 0;

	curs = figure_new_point(x, y);

	node = lptr;
	while (node && !end) {
		fptr = list_get_data(node);

		if (!fptr) {
			node = node->next;

			continue;
		}

		switch (fptr->type) {
			case FG_TYPE_POINT:
				if (gel_is_point_in_point(fptr, curs))
					end = 1;
			break;

			case FG_TYPE_LINE_PP:
				if (gel_is_point_in_line(fptr, curs))
					end = 1;
			break;

			case FG_TYPE_RECT_PP:
				if (gel_is_point_in_rect(fptr, curs))
					end = 1;
			break;

			case FG_TYPE_CIRCLE:
				if (gel_is_point_in_circle(fptr, curs))
					end = 1;
			break;
		}

		if (end) {
			fptr->visible = VM_SELECTED;
			last_selected_figure = fptr;

			break;
		}

		node = node->next;
	}

	figure_free(curs);

	if (!end)
		ch_click_cursor_unselect_all(draw_area, lptr, x, y);

	gtk_widget_queue_draw(draw_area);
}

void ch_click_cursor_unselect_all(GtkWidget *draw_area, list *lptr, double x, double y) {
	list_crawl(lptr, unselect);

	gtk_widget_queue_draw(draw_area);
}

void ch_unselect_last() {
	last_selected_figure->visible = VM_SHOW;
}