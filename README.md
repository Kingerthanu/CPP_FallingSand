# CPP_FallingSand
New Patch Yippe! 

https://tenor.com/mn1pxi8wy2a.gif

 This Patch Works To Sand Out A Lot Of Features Into More Intuitive Design; This Includes Bouncing Of Bombs On Walls When Thrown, Sinking In Water And Moving Through Smoke Cells. I Also Fixed Molten/Tindered Shrapnel Cells From Explosions In Order To Allow Extinguishment By Adding Some Checks On Our Cells During Movement Calls. Every Cell Other Than Stone Should Be Able To Shift Around Eachother As Well To Allow For Fluid Movement, And Because Sand Is Granualted It Will Have A Dynamic Way Of Piling Up, Shifting Sideways Proportionally To Our Y-Axis Velocity. Throwables Still Will Have The Ability To Freeze During ANY Collision To Allow Sticking But Should Be Aware Of It As Can Look/Act A Little Janky But I Likey So It Stays.

The Program Is In A Stable And Satisfying State As A Lot Of New Things I Want To Add Are Derivatives Of Our Base Behaivors Of Our Given Classes; I Know I Should Be Including Interfaces For Our Shared Behaivor Like CheckPoint In Motion Of Entities (But Equally Know We Could Add Some Unneccesary Bloat On Needing To Check For Dynamic Definiitons On a Closed-Codebase). I Know These Optimizations/Changes Can Be Figured Out Later In Time And Would Help Review When Coming Back To This Project So Want To Leave It Here.
