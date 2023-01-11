<h1>Robot project</h1>


<h3>Introduction</h3>

This project is a simulation of a robot in an environment that you choose to train a neural network through genetic algorithm and when the neural network is trained in a simulation<br/>
Make it work in a real environment<br/>


<h3>Installation</h3>

You have to install the sdl and sdl_ttf to make it work<br/>
<code> ~$ sudo apt-get install build-essential libsdl-dev libsdl-ttf2.0-dev</code></br>


Then you just have to use the makefile<br/>
<code> ~$ make</code></br>


<h3>How make it work</h3>

You have multiple program that you can use<br/>
In one hand, you have the program <mark>app_map</mark> in the bin folder<br/>
You can launch it with the command line <code> ~$ ./app_map</code><br/>
This program helps you to create some map where the robot will train and develop some abilities with all the difficulties it will meet in the environment<br/>
<br/>

You also have the main program which is <mark>app_training</mark> still in the bin folder<br/>
You can launch it with the command line <code> ~$ ./app_training</code><br/>
And even put an neural network already train in the universe, you created with the app_map program<br/>
To make it, you have to specify the path to the neural network's file in argument<br/>
For example, <code> ~$ ./app_training ../resources/trained_model/brain_599071.ml</code><br/>
It will launch the simulation but the first neural network to train will be the neural network specify<br/>
![My Images](documentation/4.png)

<h3>How to make map (with app_map)</h3>

![My Images](documentation/5.png)

There are some maps already made in the project, if you watch the code you can add map or even remove one of them.<br/>
If you want to create a new map, you have to launch the program app_map<br/>
Then you can use buttons to create the map<br/>
Here the list of them :<br/>
<table>
    <tr>
        <th>Button</th>
        <th>Use</th>
    </tr>
    <tr>
        <th>mousebutton_left</th>
        <th>White line(the robot will be dead if it goes on it)</th>
    </tr>
    <tr>
        <th>mousebutton_right</th>
        <th>Red Line(the robot will be dead if it goes on it) same as white but red</th>
    </tr>
    <tr>
        <th>mousebutton_left&&space_key</th>
        <th>Green Line(the robot will win the game if it reaches it)</th>
    </tr>
    <tr>
        <th>mousebutton_left&&u_key</th>
        <th>Point where the robot will appear on the map(the robot's orientation is random)</th>
    </tr>
    <tr>
        <th>enter_key</th>
        <th>It saves the map</th>
    </tr>
    <tr>
        <th>esc_key(one time)</th>
        <th>It will cancel the line you are making</th>
    </tr>
    <tr>
        <th>esc_key(two time)</th>
        <th>It will remove all the lines and points on the map</th>
    </tr>
</table>

Finally when you have save the map, you have to rename the map in the folder resources, here <mark>resources/map/map.level</mark><br/>
You rename it for example map_5.level and after that you add the map to the code in main_training.cpp after line 123 <code>universe.addLevel("../resources/map_5.level");</code><br/>
Then you will have your new level and all the position in the robot's training.<br/>


<h3>Training</h3>

![My Images](documentation/3.png)

The training is done with the program <mark>app_training</mark><br/>
The buttons you have to know to use the program are listed here:<br/>
<table>
    <tr>
        <th>Button</th>
        <th>Use</th>
    </tr>
    <tr>
        <th>esc_key</th>
        <th>quit the program</th>
    </tr>
    <tr>
        <th>space_button</th>
        <th>Allow you to slow down the training and eventually watch the behavior of the robot</th>
    </tr>
    <tr>
        <th>enter_key</th>
        <th>save the best neural_network into a file where name of it is brain_{score}.ml</th>
    </tr>
</table>

and like said before you can reuse your neural network which were trained before, if you specifies it in argument<br/>


<h3>How it works</h3>

It uses a genetic algorithm<br/>
Which means it create a population (here it's a population of neural network with values randomly set)<br/>
We wants this population to evolve so we will apply some function to them<br/>
Firstly, we will score each of them and make a list where the strongest is the first one(high score) and the last one is the worst one(with the lower score)<br/>
Then we apply a selection based on the score and we make them reproduce(which mean we mix theirs values) to create a new neural network and we add it on the next population<br/>
One we have new babies, we add some mutations on them (it will change a little bit of theirs values inside the neural network)<br/>
But we keep the first one without mutations in the new population<br/>
And we fill the rest of the population with random new neural network<br/>
<br/>
We are on the next generation here, we do it in loop and it will evolve to create smart robot<br/>



<h3>Transfer To Arduino</h3>

The neural network is trained so we want to transfer it to arduino<br/>
To make it, we will use the program <mark>extract_nn</mark><br/>
If it is not made, you just have to run<code> ~$ make</code><br/>
So now, you just have to go the bin folder and run <code> ~$ ./extract_nn</code><br/>
You will get this.<br/>

![My Images](documentation/6.png)

Then you just have to copy and paste on your arduino program.<br/>

![My Images](documentation/7.png)

It will load the neural_network trained on your computer on your robot !<br/>

<h3>Graph of the evolution</h3>

There is a program <mark>display_stat.py</mark><br/>
Which you can use to see the evolution of each of your neural network<br/>
Give the program the right to be launch <code> ~$ chmod +x display_stat.py </code><br/>
You just have to launch it, <code> ~$ ./display_stat.py </code><br/>
Then you will see this<br/>

![My Images](documentation/8.png)

Once, you have choose your file, it will display the evolution<br/>

![My Images](documentation/9.png)

<br/>
Have funnn!

