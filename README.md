# 1. Requirements

- To make a 3D Tetris game
- The size for the stack of blocks should be greater than or equal to 5 x 10 x 5
- A block keeps going down by one cell every one second.
- The interactive viewing control must be supported.
- Block rotation and position control.

# 2. Implemented

### 2.1 Block Primitive

  Block class is the class has block type(L, I, B, N), cube primitive’s vertex position, vertex colors,
vao, and vbos. According to block type, by calling its draw method, draw the cube at different position.

**2.1.1 Member Method**

- `draw()` : draw the block

### 2.2 BlockManager

  BlockManager class has a pointer of Block, StackManager, rotation transformation matrix,
translate transforming matrix, and BlockState.
It tracks position and cubes’ position of moving block by updating BlockState.

**2.2.1 Member Method**

- `glm::mat4 get_R()` : returns the rotation transforming matrix.
- `glm::mat4 get_T()` : returns the translate transforming matrix.
- `int move(int dx, int dy, int dz)` : updates the block position and cubes position of this block, and
returns status cod(int) according to after moving status(0 : need to check game over condition, 1:
stacked condition, 2: moving condition, 3: cannot move condition)
- `void rotate_x()` : updates cubes of block position by rotating on x-axis and rotation transforming
matrix
- `void rotate_y()` : updates cubes of block position by rotating on y-axis and rotation transforming
matrix
Term Project: 3D Tetris
- `void rotate_z()` : updates cubes of block position by rotating on z-axis and rotation transforming matrix
- `char rand_type()` : returns random type of block(‘L’, ‘I’, ‘B’, ‘N’) used when new block should be
created.
- `void newBlock()` : set a new type of block by calling rand_type, and reset the position of block, its cubes, rotation transforming matrix, and translate transforming matrix.

## 2.3 StackManager

  StackManager class manages the stack. It has pointer of CubePrimitive, and occupied map. By
referencing occupied map, it tracks the stacked cubes’ position and draw the cube.

### 2.3.1 Member Method

- `glm::vec3 index2pos(int i)` : return the cube’s position according to index of occupied map.
- `pos2index(const glm::vec3& pos)` : return the index of occupied map according to cube’s position.
- `void push_cube(glm::vec3& pos)` : updated occupied map according to pushed cube’s position.
- `bool isOccupied(glm::vec3& pos)` : according to position, tell that position is occupied or not.
- `void draw()` : draw the cube on screen by referencing occupied map.
- `void draw_wire()` : according to value of show_wireframe, it draws wireframe of cube in stack.
- 

## 2.3 GameManager

  GameManager class manages the game. It has coin, score, pointer of StackManager, pointer of
BlockManager.

### 2.3.1 Member Method

- `void clearLayer(int y)` : clear the layer and update the occupied map of upper layer of cleared
layer.
- `void calScore()` : when stacked condition occurs, it checks if gets a score or not. If can get score
it clears layer that is completely filled by calling clearLayer method
- `void restart()` : resets the occupied map of StackManager, block position by calling newBlock()
of BlockManager
- `bool gameOver()` : checks the game over condition(block is stacked over the top of stack) and
shows score and replay message.

# 3. Missing Function

- Drop the block when press ‘space bar’
    - rather than implementing this function, I choose to make if player press space bar(‘ ‘), the block falls
    down by 1 block.

# 4. How to Play

### 4.1 Basic Rule

- A block keeps going down by one cell every one second.
-The moving block touches the ground or any other existing block below it, it is stacked.
- If the stack of block is greater than the maximum height, the game is over.
- The game score is the total number of layers that have been cleared from the stack.
- As soon as any layer of the stack is completely filled with blocks, it should be cleared.

### 4.2 Basic Control Key

- **Move Block(Arrow Key and Space Bar)**

  ![MoveBlock](https://user-images.githubusercontent.com/77388214/204170423-c7aaa206-4163-4455-8ff3-464caeba011a.png)

- **Rotate Block**

    -‘a’ key : Rotate a block about the x-axis by 90 degrees.
    
    ![A_Key](https://user-images.githubusercontent.com/77388214/204170503-c89a750f-9e08-422a-b71f-077a4bfead2c.png)
    
    -‘s’ key : Rotate a block about the x-axis by 90 degrees.
    
    ![S_Key](https://user-images.githubusercontent.com/77388214/204170559-6cf880c8-bca0-46d8-988c-e72a7a22ba89.png)
    
    -‘d’ key : Rotate a block about the x-axis by 90 degrees.
    
    ![D_Key](https://user-images.githubusercontent.com/77388214/204170593-a6dc296b-1cc1-4303-a68c-517d868d1f6c.png)
    

- **Show Coordinate System**
    
    - ‘c’ key : Show the axis of coordinate system
    
    ![C_Key](https://user-images.githubusercontent.com/77388214/204170622-a3d84a5f-91cb-42cb-a2b8-9bf29b79114d.png)
    
- **Exit the Game(‘e’ key)**
    
    - press ‘e’ key : exit the game.
    
- **Interactive Viewing Control**
    - Mouse Left Button + Alt + Drag : Tumble Tool
    - Mouse Middle Button + Alt + Drag : Track Tool
    - Scroll up / down : Zoom Tool
    - Scroll up / down + Alt : Dolly Tool
