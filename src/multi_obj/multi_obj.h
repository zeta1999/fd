#ifndef __FLAT_DRAW_MULTI_OBJECT_PROCESSING_LIBRARY_HEADER_FILE_INCLUDED__
#define __FLAT_DRAW_MULTI_OBJECT_PROCESSING_LIBRARY_HEADER_FILE_INCLUDED__

void mol_free_from_node(list *lptr);
void mol_draw_obj_from_node(list *lptr);
figure *mol_conv_to_figure(list *lptr);

#endif
