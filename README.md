# CPP_FallingSand
This Patch Consists of A New Hud For Type Selection as well as a radial slider for adding big amounts of a cell types. New Velocity Traversal Function Is Silly And Allowed Me To Add Smoke Cell Types. I Also Added A Wrapper Class Around Our "Cell World" To Hold Better
Encapsulation Between Our Driver And Our Actual World. We Also Added In A global userCursor Which Kinda Seems A Little Unsafe But Holds Current User Stats For Easy Access And Holds A Lot of States of The User. We Also Added Bombs Which Are Fun And Work As A New World Entity
Type Which Basically Is A Container For Cells In The Grid Which Represent this->entity. Bombs Shoot Out Smoke Cells Outward As Well As Their Own Shrapnel Of Their Shell. These Bombs Also Charr Surrounding Solid Blocks.

Really Fun New Applications And Understandings Of Class Hierachies And Management Of Data. In New UI Am Using Single Buffer In Which Has Stages For Calling Either The Closed Or Open State; While This Does Impose Some "Hard-Codyness" By Implying That The Data Will Be Perfect Coming
In But Now Knowing Of Programming By Contract Probabaly Will Try And Properly Outline My Docs To Show Proper Usage Instead Of Defending From It (At Least When Necessary; Im Not Gonna Want To Trust a Random User To Alter userCursor Or Say By Contract Dont Do That But Instead
Imply Contract With Velocity Traversals)
For The Bomb We Made A Associated Dynamic_Cell Class Type In Which Inherits From Dynamic_NonSolid_Cell For Later Application In Shifting Our Associated Cells (Bomb Falling). 
