// int elf_kit_get_char(elState *S);
// int elf_kit_get_mouse_pos(elState *S) {
// 	int x,y;
// 	kit_mouse_pos(ctx,&x,&y);
// 	elf_Table *tab = elf_new_table(S);
// 	elf_tsets_int(tab,elf_new_string(S,"x"),x);
// 	elf_tsets_int(tab,elf_new_string(S,"y"),y);
// 	elf_add_tab(S,tab);
// 	return 1;
// }
// int elf_kit_key_down(elState *S) {
// 	int key = elf_get_int(S,0);
// 	elf_add_int(S,kit_key_down(ctx,key));
// 	return 1;
// }
// int elf_kit_key_pressed(elState *S) {
// 	int key = elf_get_int(S,0);
// 	elf_add_int(S,kit_key_pressed(ctx,key));
// 	return 1;
// }
// int elf_kit_key_released(elState *S) {
// 	int key = elf_get_int(S,0);
// 	elf_add_int(S,kit_key_released(ctx,key));
// 	return 1;
// }
// int elf_kit_mouse_down(elState *S) {
// 	int key = elf_get_int(S,0);
// 	elf_add_int(S,kit_key_released(ctx,key));
// 	return 1;
// }
// int elf_kit_mouse_pos(elState *S, int *x, int *y);
// int elf_kit_mouse_delta(elState *S, int *x, int *y);
// int elf_kit_mouse_down(elState *S, int button);
// int elf_kit_mouse_pressed(elState *S, int button);
// int elf_kit_mouse_released(elState *S, int button);
