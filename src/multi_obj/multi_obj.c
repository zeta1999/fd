#include <gtk/gtk.h>

#include "../list/list.h"
#include "../figure/figure.h"
#include "../text/text.h"
#include "../draw/draw.h"

void mol_free_from_node(list *lptr) {
	text *tptr;

	if (!lptr || !lptr->data)
		return ;

	switch (lptr->dt) {
		case OT_FIGURE:
			figure_free_list(lptr);
		break;

		case OT_TEXT:
			tptr = (text*)lptr->data;

			if (tptr->buffer)
				tl_free_buffer(tptr);
			tl_free(tptr);
		break;
	}
}

void mol_draw_obj_from_node(list *lptr) {
	if (!lptr || !lptr->data)
		return ;

	switch(lptr->dt) {
		case OT_FIGURE:
			dl_draw_figure((figure*)lptr->data);
		break;

		case OT_TEXT:
			if (((text*)(lptr->data))->visible != VM_NOT_FINISHED)
				dl_draw_text((text*)lptr->data);
		break;
	}
}

figure *mol_conv_to_figure(list *lptr) {
	figure *fptr;
	text *tptr;

	if (!lptr || !lptr->data)
		return NULL;

	if (lptr->dt == OT_FIGURE)
		return (figure*)(lptr->data);
	else {
		tptr = (text*)lptr->data;
		fptr = figure_new_point(tptr->x, tptr->y);
		fptr->visible = VM_SELECTED;

		return fptr;
	}
}
