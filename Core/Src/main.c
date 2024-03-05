/* USER CODE BEGIN Header */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "init.h"
#include "motor.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif
#ifndef COL
#define COL COL
#endif
#ifndef ROW
#define ROW ROW
#endif

#define checknorth ((thisnode->wallval >> 0)%2)
#define checkeast  ((thisnode->wallval >> 1)%2)
#define checksouth ((thisnode->wallval >> 2)%2)
#define checkwest  ((thisnode->wallval >> 3)%2)
#define NORTH 1
#define EAST 2
#define SOUTH 4
#define WEST 8 
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;

/* USER CODE BEGIN PV */
const char Wall = 'x';
const char Path = ' ';
uint16_t RSensor, FSensor, LSensor;
uint8_t check = 0;
uint16_t tempsen;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */
void delay_us(uint16_t us){
	__HAL_TIM_SetCounter(&htim1, 0);
	while(__HAL_TIM_GetCounter(&htim1) < us);
}

void trigger1(){
	HAL_GPIO_WritePin(TRIG_FRONT_GPIO_Port, TRIG_FRONT_Pin, 1);
	delay_us(1000);
	HAL_GPIO_WritePin(TRIG_FRONT_GPIO_Port, TRIG_FRONT_Pin, 0);
}
void trigger2(){
	HAL_GPIO_WritePin(TRIG_LEFT_GPIO_Port, TRIG_LEFT_Pin, 1);
	delay_us(1000);
	HAL_GPIO_WritePin(TRIG_LEFT_GPIO_Port, TRIG_LEFT_Pin, 0);
}
void trigger3(){
	HAL_GPIO_WritePin(TRIG_RIGHT_GPIO_Port, TRIG_RIGHT_Pin, 1);
	delay_us(1000);
	HAL_GPIO_WritePin(TRIG_RIGHT_GPIO_Port, TRIG_RIGHT_Pin, 0);
}

Node *newnode(uint8_t row, uint8_t col){
    Node *thisnode;
    thisnode = (Node*) malloc(sizeof(Node));
    ROW = row;
    COL = col;
    thisnode->visit = false;
    thisnode->wallval = 0;
    return thisnode;
}

Maze *newmaze(){
    Maze *thismaze;
    thismaze = (Maze*) malloc(sizeof(Maze));
    for (int row = 0; row<size; row++)
        for (int col = 0; col<size; col++)
            thismaze->map[row][col] = newnode(row,col);
            
    return thismaze;
}

Stack *newstack(){
    Stack *thisstack;
    thisstack = (Stack*) malloc(sizeof(Stack));
    thisstack->index = 0;
    return thisstack;
}


uint8_t checkgoal(Node *thisnode){
    if(ROW == size/2 || ROW == (size/2)-1)
        if(COL == size/2 || COL == (size/2)-1)
            return true;
    return false;
}

uint8_t checkstart(Node *thisnode){
    if(ROW == size-1 && COL == 0)
        return true;
    return false;    
}

uint8_t checknull(uint8_t row, uint8_t col){
    if(row < 0 || row >= size || col < 0 || col >= size)
        return 1;
    else return 0;
}

void deleteNode(Node **thisnode){
    free (*thisnode);
    *thisnode = 0;
}

void deleteMaze(Maze **thismaze){
    for (int row = 0; row<size; row++)
        for (int col = 0; col<size; col++)
            deleteNode(&((*thismaze)->map[row][col]));
    free (*thismaze);
    *thismaze = 0;
}

void deleteStack(Stack **thisstack){
    for (int idx = ((*thisstack)->index)-1; idx>=0 ; idx--){
        deleteNode(&((*thisstack)->Stack[idx]));
    }
    free (*thisstack);
    *thisstack = 0;
}

void push(Stack **thisstack, Node **thisnode){
    (*thisstack)->Stack[(*thisstack)->index] = *thisnode;
    (*thisstack)->index += 1;
}

void pop(Stack **thisstack, Node **thisnode){
    (*thisstack)->index -=1;
    *thisnode = (*thisstack)->Stack[(*thisstack)->index];
}

void initial_maze(Maze *maze){
    for(int i =0; i<size;i++){
        for(int j=0;j<size;j++){
            maze->map[i][j]->floodval = -1;
        }
    }
    
    maze->map[size/2][size/2]->floodval = 0;   
    maze->map[size/2 -1][size/2]->floodval = 0;
    maze->map[size/2][size/2 -1]->floodval =  0;
    maze->map[size/2 -1][size/2 -1]->floodval = 0;

    for(int i=0;i<size;i++){
        for(int j=0;j<size;j++){
            if(maze->map[i][j]->floodval !=0){
                int array[]={(abs(i-(size/2))+abs(j-(size/2))),(abs(i-(size/2 -1))+abs(j-(size/2))),(abs(i-(size/2))+abs(j-(size/2 -1))),(abs(i-(size/2 -1))+abs(j-(size/2 -1)))};
                int min =array[0];
                for(int k=1; k<4;k++){
                    if(min>array[k]){
                        min=array[k];
                    }
                }
                maze->map[i][j]->floodval=min;
                maze->map[i][j]->Mandis=min;
            }
        }
    }
}

void swap (Node **xp, Node **yp){
    Node *temp = *xp; *xp = *yp; *yp = temp;
    
} 
uint8_t findnextdir(Node *thisnode, Maze *maze, uint8_t findmin){
    uint8_t min = 16, count = 0, next_dir;
    Node *neighbor[4]; 
    if (checknull(ROW-1, COL) == 0 && !checknorth) {
        neighbor[count] = maze->map[ROW - 1][COL];
        count +=1;
    }
    if (checknull(ROW, COL+1) == 0 && !checkeast) {
        neighbor[count] = maze->map[ROW][COL + 1];
        count +=1;
    }
    if (checknull(ROW+1, COL) == 0 && !checksouth) {
        neighbor[count] = maze->map[ROW + 1][COL];
        count +=1;
    }
    if (checknull(ROW, COL-1) == 0 && !checkwest) {
        neighbor[count] = maze->map[ROW][COL - 1];
        count +=1;
    }

    for (int i = 0; i < count-1; i++){
        for (int j = 1; j < count; j++){
            if (neighbor[i]->floodval > neighbor[j]->floodval)
                swap(&(neighbor[i]), &(neighbor[j]));
            else if (neighbor[i]->floodval == neighbor[j]->floodval){
                if (neighbor[i]->visit > neighbor[j]->visit )
                    swap(&(neighbor[i]), &(neighbor[j]));
                else if (neighbor[i]->visit == neighbor[j]->visit){
                    if (neighbor[i]->Mandis > neighbor[j]->Mandis)
                        swap(&(neighbor[i]), &(neighbor[j]));
                }
            }
        }
    }

    if (findmin == true)
        return neighbor[0]->floodval;
    
    if (maze->map[ROW - 1][COL] == neighbor[0]) next_dir = NORTH;
    else if (maze->map[ROW][COL + 1] == neighbor[0]) next_dir = EAST;
    else if (maze->map[ROW + 1][COL] == neighbor[0]) next_dir = SOUTH;
    else if (maze->map[ROW][COL - 1] == neighbor[0]) next_dir = WEST;
    return next_dir;
}

void moveto(Node **pos, uint8_t *dir, Maze *maze, uint8_t next_dir){
    uint8_t rotation;
    rotation = (4*(*dir))/next_dir;
    if(rotation == 0)
        rotation = 8;
    if(rotation == 32)
        rotation = 2;
    switch(rotation){
        case 2: 
            TurnRight();
						HAL_Delay(200);
            break;
        case 8: 
            TurnLeft();
						HAL_Delay(200);
            break;
        case 1: 
        case 16: 
            Turn180();
						HAL_Delay(200);

        case 4: 
            break;
        default: ;
    }
	uint16_t addi = 0;

	switch(next_dir){
		case NORTH:
			if(((*pos)->wallval >> 2)%2 == true)
				{
					MoveBackward();
					HAL_Delay(410);
					Stop();
					HAL_Delay(100);
					addi = 40;
				}				
			break;
		case SOUTH:
			if(((*pos)->wallval >> 0)%2 == true)
				{
					MoveBackward();
					HAL_Delay(410);
					Stop();
					HAL_Delay(100);
					addi = 40;
				}
				break;
		case EAST:
			if(((*pos)->wallval >> 3)%2 == true)
				{
					MoveBackward();
					HAL_Delay(410);
					Stop();
					HAL_Delay(100);
					addi = 40;
				}
				break;
		case WEST:
			if(((*pos)->wallval >> 1)%2 == true)
				{
					MoveBackward();
					HAL_Delay(410);
					Stop();
					HAL_Delay(100);
					addi = 40;
				}
				break;

	}

/*	
	uint16_t start = HAL_GetTick();

	while(HAL_GetTick() - start < (600+addi)){
		uint8_t count = 0;
		LSensor = RSensor = 0;
			MoveForward();
		for (; count <= 20; count++){
		trigger2();
		while(HAL_GPIO_ReadPin(ECHO_LEFT_GPIO_Port, ECHO_LEFT_Pin) == 0){check = 1;}
		check = 0;
		__HAL_TIM_SetCounter(&htim1, 0);
		while(HAL_GPIO_ReadPin(ECHO_LEFT_GPIO_Port, ECHO_LEFT_Pin) == 1){}
		LSensor += __HAL_TIM_GetCounter(&htim1)/5.8;
		HAL_Delay(10);

		//
		trigger3();
		while(HAL_GPIO_ReadPin(ECHO_RIGHT_GPIO_Port, ECHO_RIGHT_Pin) == 0){check = 1;}
		check = 0;
		__HAL_TIM_SetCounter(&htim1, 0);
		while(HAL_GPIO_ReadPin(ECHO_RIGHT_GPIO_Port, ECHO_RIGHT_Pin) == 1){}
		RSensor += __HAL_TIM_GetCounter(&htim1)/5.8;
		HAL_Delay(10);
		}
		CheckForward(LSensor, RSensor);
		HAL_Delay(50);
	}
*/
	MoveForward();
	HAL_Delay(845+addi);
//	HAL_Delay(710+addi/10);
	Stop();
	
	trigger1();
	while(HAL_GPIO_ReadPin(ECHO_FRONT_GPIO_Port, ECHO_FRONT_Pin) == 0){check = 1;}
	check = 0;
	__HAL_TIM_SetCounter(&htim1, 0);
	while(HAL_GPIO_ReadPin(ECHO_FRONT_GPIO_Port, ECHO_FRONT_Pin) == 1){}
	FSensor = __HAL_TIM_GetCounter(&htim1)/5.8;
	HAL_Delay(10);
	if(FSensor < 100){
		MoveForward();
		HAL_Delay(420);
		Stop();
		HAL_Delay(80);
		MoveBackwardSlow();
		HAL_Delay(170);
		Stop();	
	}
	
	
	addi = 0;
	
    *dir = next_dir;
    switch(next_dir){
        case NORTH:
            *pos = maze->map[(*pos)->row - 1][(*pos)->col];
            break;
        case EAST:
            *pos = maze->map[(*pos)->row][(*pos)->col + 1];
            break;
        case SOUTH:
            *pos = maze->map[(*pos)->row + 1][(*pos)->col];
            break;
        case WEST:
            *pos = maze->map[(*pos)->row][(*pos)->col - 1];
    }
}

void recalculate(Node *thisnode, Maze *maze, Stack *stack){
    push(&stack, &thisnode);
    do{
        pop(&stack, &thisnode);
        uint8_t min = findnextdir(thisnode, maze, true);
        if(thisnode->floodval <= min){
            thisnode->floodval = min+1;
            if(checknull(ROW, COL-1) == 0 && !checkwest){
                push(&stack,&(maze->map[ROW][COL - 1]));
            }
            if(checknull(ROW+1, COL) == 0 && !checksouth){
                push(&stack,&(maze->map[ROW + 1][COL]));
            }
            if(checknull(ROW, COL+1) == 0 && !checkeast){
                push(&stack,&(maze->map[ROW][COL + 1]));
            }
            if(checknull(ROW-1, COL) == 0 && !checknorth){
                push(&stack,&(maze->map[ROW - 1][COL]));
            }
        }
    }
    while(stack->index != 0);
}

uint8_t checkstack(Stack *thisstack, Node *thisnode){
    for (int i = 0; i < thisstack->index; i++)
        if (thisnode == (thisstack->Stack[i]))
            return 0;
    return 1;
}

void reflood(Maze *maze, uint8_t togoal){
    Stack *stack1 = newstack(), *stack2 = newstack();
    Node *thisnode;
    uint8_t i = 0;
    for(int i =0; i<size;i++){
        for(int j=0;j<size;j++){
            maze->map[i][j]->floodval = 255;
        }
    }

    if (togoal == true){ 
        maze->map[size/2][size/2]->floodval = 0;   
        maze->map[size/2 -1][size/2]->floodval = 0;
        maze->map[size/2][size/2 -1]->floodval =  0;
        maze->map[size/2 -1][size/2 -1]->floodval = 0;
        
        push(&stack1, &(maze->map[size/2][size/2-1]));
        push(&stack1, &(maze->map[size/2][size/2]));
        push(&stack1, &(maze->map[size/2-1][size/2-1]));
        push(&stack1, &(maze->map[size/2-1][size/2]));
    }

    else {
        maze->map[size-1][0]->floodval = 0;
        push(&stack1, &(maze->map[size-1][0]));
    }

    do {
        do {
            pop(&stack1, &thisnode);
            if(checknull(ROW, COL - 1) == 0 && !checkwest  && (maze->map[ROW][COL - 1]->floodval)>i+1 && checkstack(stack2, maze->map[ROW][COL - 1])){
                maze->map[ROW][COL - 1]->floodval = i+1;
                push(&stack2,&(maze->map[ROW][COL - 1]));
            }
            if(checknull(ROW + 1, COL) == 0 && !checksouth && (maze->map[ROW + 1][COL]->floodval)>i+1 && checkstack(stack2, maze->map[ROW + 1][COL])){
                maze->map[ROW + 1][COL]->floodval = i+1;
                push(&stack2,&(maze->map[ROW + 1][COL]));
            }
            if(checknull(ROW, COL + 1) == 0 && !checkeast  && (maze->map[ROW][COL + 1]->floodval)>i+1 && checkstack(stack2, maze->map[ROW][COL + 1])){
                maze->map[ROW][COL + 1]->floodval = i+1;
                push(&stack2,&(maze->map[ROW][COL + 1]));
            }
            if(checknull(ROW - 1, COL) == 0 && !checknorth && (maze->map[ROW - 1][COL]->floodval)>i+1 && checkstack(stack2, maze->map[ROW - 1][COL])){
                maze->map[ROW - 1][COL]->floodval = i+1;
                push(&stack2,&(maze->map[ROW - 1][COL]));
            }
        } while (stack1->index != 0);
        Stack *temp = stack1; 
        stack1 = stack2;
        stack2 = temp;
        i++;
    } while (stack1->index != 0);

    deleteStack(&stack1);
    deleteStack(&stack2);
}

void updatecell(Node *thisnode, Maze *maze, uint8_t dir){
    switch (dir)
    {
    case SOUTH:
        if (checknull(ROW+1, COL) == 0) 
            maze->map[ROW + 1][COL]->wallval = maze->map[ROW + 1][COL]->wallval | NORTH;
        break;
    case NORTH:
        if (checknull(ROW-1, COL) == 0) 
            maze->map[ROW - 1][COL]->wallval = maze->map[ROW - 1][COL]->wallval | SOUTH;
        break;
    case EAST:
        if (checknull(ROW, COL+1) == 0) 
            maze->map[ROW][COL + 1]->wallval = maze->map[ROW][COL + 1]->wallval | WEST;
        break;
    case WEST:
        if (checknull(ROW, COL-1) == 0) 
            maze->map[ROW][COL - 1]->wallval = maze->map[ROW][COL - 1]->wallval | EAST;
        break;
    default:
        break;
    };
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start(&htim1);
	
	HAL_TIM_Base_Start(&htim3);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);	

	char maze_text[2*size + 1][2*size + 1] = {
{'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x'	 }, 
{'x',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	'x',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	'x'	 }, 
{'x',	' ',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	' ',	'x',	'x',	'x',	' ',	'x',	'x',	'x',	' ',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	' ',	'x',	' ',	'x',	' ',	'x'	 }, 
{'x',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	'x',	' ',	' ',	' ',	'x',	' ',	'x',	' ',	' ',	' ',	' ',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x'	 }, 
{'x',	' ',	'x',	' ',	'x',	' ',	'x',	'x',	'x',	'x',	'x',	' ',	'x',	' ',	'x',	'x',	'x',	' ',	'x',	' ',	'x',	'x',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x'	 }, 
{'x',	' ',	'x',	' ',	'x',	' ',	' ',	' ',	' ',	' ',	'x',	' ',	'x',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	'x',	' ',	'x'	 }, 
{'x',	' ',	'x',	' ',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	' ',	'x',	' ',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	' ',	'x',	' ',	'x',	' ',	'x'	 }, 
{'x',	' ',	' ',	' ',	'x',	' ',	' ',	' ',	' ',	' ',	'x',	' ',	' ',	' ',	'x',	' ',	' ',	' ',	'x',	' ',	' ',	' ',	' ',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	' ',	' ',	'x'	 }, 
{'x',	'x',	'x',	' ',	'x',	' ',	'x',	'x',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	'x',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	'x',	'x',	'x',	'x'	 }, 
{'x',	' ',	' ',	' ',	'x',	' ',	'x',	' ',	' ',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	' ',	' ',	'x',	' ',	' ',	' ',	'x',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	'x'	 }, 
{'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	'x',	'x',	' ',	'x',	'x',	'x',	' ',	'x',	'x',	'x',	'x',	'x',	' ',	'x',	' ',	'x'	 }, 
{'x',	' ',	' ',	' ',	'x',	' ',	' ',	' ',	'x',	' ',	' ',	' ',	'x',	' ',	'x',	' ',	' ',	' ',	' ',	' ',	'x',	' ',	'x',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	'x',	' ',	'x'	 }, 
{'x',	' ',	'x',	'x',	'x',	' ',	'x',	'x',	'x',	'x',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	'x',	'x',	'x',	'x',	' ',	'x',	' ',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	' ',	'x'	 }, 
{'x',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	'x',	' ',	'x',	' ',	' ',	' ',	'x',	' ',	'x',	' ',	' ',	' ',	'x',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	'x'	 }, 
{'x',	' ',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	' ',	'x',	'x',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	'x',	'x',	' ',	'x',	'x',	'x',	'x',	'x',	' ',	'x'	 }, 
{'x',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	'x',	' ',	' ',	' ',	'x',	' ',	' ',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	' ',	' ',	'x',	' ',	' ',	' ',	' ',	' ',	'x'	 }, 
{'x',	' ',	'x',	'x',	'x',	' ',	'x',	'x',	'x',	' ',	'x',	' ',	'x',	'x',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	'x',	'x'	 }, 
{'x',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	'x',	' ',	'x',	' ',	' ',	' ',	'x',	' ',	' ',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	' ',	' ',	'x'	 }, 
{'x',	' ',	'x',	' ',	'x',	'x',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	'x',	'x',	'x',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	'x',	'x'	 }, 
{'x',	' ',	'x',	' ',	'x',	' ',	' ',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	'x',	' ',	' ',	' ',	'x',	' ',	' ',	' ',	' ',	' ',	'x'	 }, 
{'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	' ',	'x',	' ',	'x',	'x',	'x',	' ',	'x'	 }, 
{'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	' ',	' ',	'x',	' ',	' ',	' ',	'x',	' ',	'x',	' ',	' ',	' ',	' ',	' ',	'x',	' ',	' ',	' ',	'x',	' ',	'x',	' ',	' ',	' ',	'x'	 }, 
{'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	'x',	'x',	' ',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	' ',	'x',	'x',	'x',	' ',	'x',	' ',	'x',	' ',	'x'	 }, 
{'x',	' ',	'x',	' ',	' ',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	'x',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	'x',	' ',	'x'	 }, 
{'x',	' ',	'x',	' ',	'x',	'x',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	'x',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	' ',	'x',	'x',	'x',	' ',	'x'	 }, 
{'x',	' ',	'x',	' ',	' ',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	' ',	' ',	'x',	' ',	'x',	' ',	' ',	' ',	'x',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	'x'	 }, 
{'x',	' ',	'x',	'x',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	'x',	'x',	' ',	'x',	' ',	'x',	'x',	'x',	' ',	'x',	'x',	'x',	' ',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x'	 }, 
{'x',	' ',	'x',	' ',	' ',	' ',	'x',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	'x',	' ',	' ',	' ',	'x',	' ',	'x',	' ',	' ',	' ',	'x',	' ',	' ',	' ',	' ',	' ',	'x',	' ',	'x'	 }, 
{'x',	' ',	'x',	' ',	'x',	'x',	'x',	' ',	'x',	'x',	'x',	'x',	'x',	' ',	'x',	' ',	'x',	'x',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	'x',	'x',	'x',	'x',	' ',	'x',	' ',	'x'	 }, 
{'x',	' ',	' ',	' ',	' ',	' ',	'x',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	'x',	' ',	' ',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	'x'	 }, 
{'x',	' ',	'x',	'x',	'x',	' ',	'x',	' ',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	' ',	'x',	'x',	'x',	' ',	'x'	 }, 
{'x',	' ',	'x',	' ',	' ',	' ',	'x',	' ',	'x',	' ',	' ',	' ',	' ',	' ',	' ',	' ',	'x',	' ',	' ',	' ',	'x',	' ',	' ',	' ',	'x',	' ',	'x',	' ',	' ',	' ',	' ',	' ',	'x'	 }, 
{'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x',	'x'	 }, 

};

	Maze *maze = newmaze();
	Stack *stack = newstack();
	Node *pos, *nextnode;
	initial_maze(maze);
	uint8_t dir, next_dir, movecount, goalcount, togoal = true;
	pos = maze->map[size-1][0];
	dir = NORTH;
	HAL_Delay(1000);
	HAL_GPIO_WritePin(intLED_GPIO_Port, intLED_Pin, 0);
	HAL_Delay(500);
	HAL_GPIO_WritePin(intLED_GPIO_Port, intLED_Pin, 1);		
	uint8_t readmaptext = 0, min;
	uint8_t ButtonPressed;

	while(0)
 {	
		MoveForward();

	 } /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	
	 
	 while (1){
		if(0){
			HAL_Delay(20);
			if(HAL_GPIO_ReadPin(GPIOA, Button_Press_Pin)){
				pos->visit = false;
				pos = maze->map[size-1][0];
				dir = NORTH;
				while(HAL_GPIO_ReadPin(GPIOA, Button_Press_Pin));
				HAL_Delay(1000);
			}
		}
	HAL_Delay(800);
	//read sensor
	trigger1();
	while(HAL_GPIO_ReadPin(ECHO_FRONT_GPIO_Port, ECHO_FRONT_Pin) == 0){check = 1;}
	check = 0;
	__HAL_TIM_SetCounter(&htim1, 0);
	while(HAL_GPIO_ReadPin(ECHO_FRONT_GPIO_Port, ECHO_FRONT_Pin) == 1){}
	FSensor = __HAL_TIM_GetCounter(&htim1)/58;
	HAL_Delay(10);

	//
	trigger2();
	while(HAL_GPIO_ReadPin(ECHO_LEFT_GPIO_Port, ECHO_LEFT_Pin) == 0){check = 1;}
	check = 0;
	__HAL_TIM_SetCounter(&htim1, 0);
	while(HAL_GPIO_ReadPin(ECHO_LEFT_GPIO_Port, ECHO_LEFT_Pin) == 1){}
	LSensor = __HAL_TIM_GetCounter(&htim1)/58;
	HAL_Delay(10);

	//
	trigger3();
	while(HAL_GPIO_ReadPin(ECHO_RIGHT_GPIO_Port, ECHO_RIGHT_Pin) == 0){check = 1;}
	check = 0;
	__HAL_TIM_SetCounter(&htim1, 0);
	while(HAL_GPIO_ReadPin(ECHO_RIGHT_GPIO_Port, ECHO_RIGHT_Pin) == 1){}
	RSensor = __HAL_TIM_GetCounter(&htim1)/58;
	HAL_Delay(10);
		if(readmaptext == 0 & (pos->visit)==0){			
			if (FSensor <= 10) {
					pos->wallval = pos->wallval | dir;
					updatecell(pos, maze, dir);
			}
			if (RSensor <= 7) {
					pos->wallval = pos->wallval | ((dir == WEST) ? NORTH : dir*2);
					updatecell(pos, maze, dir*2);
			}
			if (LSensor <= 7) {
					pos->wallval = pos->wallval | ((dir == NORTH) ? WEST : dir/2);
					updatecell(pos, maze, dir/2);
			}
			pos->visit = true;
		}

		if(readmaptext==1 & (pos->visit)==0){
			if (maze_text[2*(pos->row)+2][2*(pos->col)+1] == Wall) {
				pos->wallval = pos->wallval | SOUTH;
				updatecell(pos, maze, SOUTH);
			}
			if (maze_text[2*(pos->row)][2*(pos->col)+1] == Wall) {
				pos->wallval = pos->wallval | NORTH;
				updatecell(pos, maze, NORTH);
			}
			if (maze_text[2*(pos->row)+1][2*(pos->col)+2] == Wall) {
				pos->wallval = pos->wallval | EAST;
				updatecell(pos, maze, EAST);
			}
			if (maze_text[2*(pos->row)+1][2*(pos->col)] == Wall) {
				pos->wallval = pos->wallval | WEST;
				updatecell(pos, maze, WEST);
			}
			pos->visit = true;
		}

		recalculate(pos, maze, stack);	
		next_dir = findnextdir(pos, maze, false);
		moveto(&pos, &dir, maze, next_dir);
			HAL_GPIO_WritePin(intLED_GPIO_Port, intLED_Pin, 0);
			HAL_Delay(100);
			HAL_GPIO_WritePin(intLED_GPIO_Port, intLED_Pin, 1);
			HAL_Delay(100);
		if(checkgoal(pos) && togoal){
			Node *temp = pos;
			do{
				if(readmaptext & !(pos->visit)){
					if (maze_text[2*(pos->row)+2][2*(pos->col)+1] == Wall) {
						pos->wallval = pos->wallval | SOUTH;
						updatecell(pos, maze, SOUTH);
					}
					if (maze_text[2*(pos->row)][2*(pos->col)+1] == Wall) {
						pos->wallval = pos->wallval | NORTH;
						updatecell(pos, maze, NORTH);
					}
					if (maze_text[2*(pos->row)+1][2*(pos->col)+2] == Wall) {
						pos->wallval = pos->wallval | EAST;
						updatecell(pos, maze, EAST);
					}
					if (maze_text[2*(pos->row)+1][2*(pos->col)] == Wall) {
						pos->wallval = pos->wallval | WEST;
						updatecell(pos, maze, WEST);
					}
					pos->visit = true;
				}
				else break;
				moveto(&pos, &dir, maze, next_dir);
				next_dir = findnextdir(pos, maze, false);
			} while(pos != temp);

			togoal = false;
			reflood(maze, togoal);
			while(1){
			HAL_GPIO_WritePin(intLED_GPIO_Port, intLED_Pin, 0);
			HAL_Delay(1000);
			HAL_GPIO_WritePin(intLED_GPIO_Port, intLED_Pin, 1);
			HAL_Delay(1000);}
	}

		if (checkstart(pos) && !togoal){
			togoal = true;
			reflood(maze, togoal);
			HAL_GPIO_WritePin(intLED_GPIO_Port, intLED_Pin, 0);
			HAL_Delay(2000);
			HAL_GPIO_WritePin(intLED_GPIO_Port, intLED_Pin, 1);
			HAL_Delay(2000);
		}

		
}

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 71;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 35;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(intLED_GPIO_Port, intLED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, INT_1_Pin|INT_2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, INT_3_Pin|INT_4_Pin|TRIG_FRONT_Pin|TRIG_LEFT_Pin
                          |TRIG_RIGHT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : intLED_Pin */
  GPIO_InitStruct.Pin = intLED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(intLED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : INT_1_Pin INT_2_Pin */
  GPIO_InitStruct.Pin = INT_1_Pin|INT_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : INT_3_Pin INT_4_Pin */
  GPIO_InitStruct.Pin = INT_3_Pin|INT_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : Button_Press_Pin */
  GPIO_InitStruct.Pin = Button_Press_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(Button_Press_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : ECHO_FRONT_Pin */
  GPIO_InitStruct.Pin = ECHO_FRONT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ECHO_FRONT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : TRIG_FRONT_Pin TRIG_LEFT_Pin TRIG_RIGHT_Pin */
  GPIO_InitStruct.Pin = TRIG_FRONT_Pin|TRIG_LEFT_Pin|TRIG_RIGHT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : ECHO_LEFT_Pin ECHO_RIGHT_Pin */
  GPIO_InitStruct.Pin = ECHO_LEFT_Pin|ECHO_RIGHT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
