#ifndef FILEOPS_H
#define FILEOPS_H

void view_piece_list();
void add_piece();
void choose_piece(int role);
void delete_piece();

void view_part_list(char* piece);
void view_part(char* piece);
void add_part(char* piece);
void append_note(char* piece);
void delete_part(char* piece);

void start_rehearsal(char* piece);
void stop_rehearsal(char* piece);
void join_rehearsal(char* piece);

#endif