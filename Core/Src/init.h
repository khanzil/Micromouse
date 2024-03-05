#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define size 16
#define COL thisnode->col
#define ROW thisnode->row
typedef struct Node {
    uint8_t col;
    uint8_t row;
    uint8_t visit;
    uint8_t wallval;
    uint16_t floodval;// khoảng cách bé nhất
    uint8_t Mandis;

}Node;

typedef struct Maze {
    Node *map[size][size];
}Maze;

typedef struct Stack{
    Node *Stack[size * size];
    uint8_t index;
}Stack;

Node *newnode(uint8_t row, uint8_t col); // Tạo Node mới
Maze *newmaze(); // Tạo Maze mới
Stack *newstack(); // Tạo Stack mới
uint8_t checkgoal(Node *pos); // Kiểm tra xem đến đích chưa
uint8_t checkstart(Node *pos); // Kiểm tra xem về Start chưa
uint8_t checknull(uint8_t row, uint8_t col);
void deleteNode(Node **thisnode); // Xoá Node
void deleteMaze(Maze **thismaze); // Xoá Maze
void deleteStack(Stack **thisstack); // Xoá Stack
void push(Stack **thisstack, Node **thisnode); // Đẩy node vào Stack
void pop(Stack **thisstack, Node **thisnode); // Lấy node từ Stack
void initial_maze(Maze *maze); // Khởi tạo giá trị flood
void swap (Node **xp, Node **yp); 
uint8_t findnextdir(Node *thisnode, Maze *maze, uint8_t findmin);
void moveto(Node **pos, uint8_t *dir, Maze *maze, uint8_t next_dir);
void printmap(Maze *thismaze, Node *pos);
void printflood(Maze *thismaze, Node *pos);
void recalculate(Node *thisnode, Maze *maze, Stack *stack); // Modified Floodfill
uint8_t checkstack(Stack *thisstack, Node *thisnode); // Kiểm tra xem Node đã có trong Stack
void reflood(Maze *maze, uint8_t togoal); // Floodfill
void updatecell(Node *thisnode, Maze *maze, uint8_t dir); // Cập nhật wallval của ô ở hướng dir của Node
