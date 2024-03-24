# CPP_FallingSand
New Patch Adds Throwables! Works Currenty With Our Dynamic_Entity Classes In Which Work As Handlers For Sets Of Cells. It Can Allow Us To Do Operations On Non-Dynamic Cells By Implementing Entity Logic For Cell Motion Which Could Be Independend From A Cell's Own Think Operation.
Currently We Have A Rock (A Single Cell Throwable), As Well As A Boulder (A Multi-Cell Throwable [Circle In This Case]). Each Of These Have A Ridgid Body, Meaning They Will Not Break Shape After Collision And Because Are Stone Will Be Hard Baked Into Their Location (future plans wish to make dynamic bodies so we can for example throw a ball of sand). 

Our New Patch Also Includes Fire Spread From Explosions! This Is Done With A New Cell Type Which Has A Chance To Spawn From A Explosion And If Collides With Sand, Will Ignite It Causing A Fire To Spread Causing Smoke To Billow And Cells To Burn Away.


Our Logic Right Now Wants To Work On A Entity Batch For Multiple Concurrent Entity Actions, But Right Now With Throwables and Explosives We Can Only Work With One At A Time (We Need To Go And Fix .Clear() As Is Over The Top For A known size 1 entity_batch At This Time). After A Entity Does It's Logic And Leaves The Batch, It's Associated Cells Should Still Stay (So If We Make A Throwable Stone Entity And It Stops Freefall, It Will Delete Itself and Only Keep It's Associated Stone Cells In The Cell Grid).

![throwables-ezgif com-optimize](https://github.com/Kingerthanu/CPP_FallingSand/assets/76754592/77a05987-3800-4137-9c76-d87af27902d6)

![shortThrow-ezgif com-video-to-gif-converter](https://github.com/Kingerthanu/CPP_FallingSand/assets/76754592/b3db7905-57dc-45f7-b11e-00f26c65535a)
