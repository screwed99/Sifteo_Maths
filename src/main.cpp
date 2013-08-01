#include <sifteo.h>
#include "assets.gen.h"
#include "./Modes/Normal.h"
#include "./Modes/Mode.h"
#include "./Modes/PlayGame.h"
#include "./GameDrawer.h"
#include "./Menu/MainMenu.cpp"
#include "./Modes/Practise.h"
#include <sifteo.h>
using namespace Sifteo;

static AssetSlot MainSlot = AssetSlot::allocate()
	.bootstrap(GameAssets);
	//.bootstrap(MenuImageAssets);

static Metadata M = Metadata()
	.title("First Game")
	.package("com.sparx.operators","1.0")
	.icon(Icon)
	.cubeRange(1,CUBE_ALLOCATION);

static AssetLoader loader;
static AssetConfiguration<1> assetConfig;
VideoBuffer gVideo[CUBE_ALLOCATION];

GameDrawer myGameDrawer;
Normal normal = Normal();
Practise practise = Practise();
//All other modes instantiated using default constructors
Mode *currMode = NULL;

int continueGame = 1;


void addMoreCubes()
{
	continueGame = 0;
	for (CubeID cube : CubeSet::connected())
    {
    	myGameDrawer.drawConnectCube(cube);
    }
}

void onDisconnect(void *x, unsigned int id)
{
	LOG("logged disconnect from cube: %d\n",id);

	if(currMode)
	{
		currMode->updateDisconnect(id);
	}
}

void onConnect(void *x, unsigned int id)
{
	LOG("logged connect from cube: %d\n",id);
	auto &vid = gVideo[id];
    vid.initMode(BG0);
    vid.attach(id);
    //vid.bg0.erase(StripeTile);
    if(currMode)
	{
		currMode->updateConnect(id);
	}

}

void onNeighbourAdd(void *x,unsigned int cube0Id, unsigned int side0,
                        unsigned int cube1Id, unsigned int side1)
{
	currMode->onNeighbourAdd(x, cube0Id, side0,
                       cube1Id, side1);
}
void onNeighbourRemove(void *x,unsigned int cube0Id, unsigned int side0,
                        unsigned int cube1Id, unsigned int side1)
{
	currMode->onNeighbourRemove(x, cube0Id, side0,
                       cube1Id, side1);
}
void onTouch(void *x,unsigned int cube)
{
	currMode->onTouch(x, cube);
}




void GameDrawer::drawOpHighlight(CubeID cube, Int2 TopLeft, Int2 OpSize, int Highlight)
{
	gVideo[cube].bg0.image(TopLeft, OpSize, Highlights[Highlight], TopLeft, 0);
}

void GameDrawer::printQuestion(CubeID cube, Int2 TopLeft, String<16> quText)
{
	gVideo[cube].bg0.text(TopLeft, Font, quText, ' ');
}

void GameDrawer::printGreenQuestion(CubeID cube, Int2 TopLeft, String<16> quText)
{
	gVideo[cube].bg0.text(TopLeft, FontGreen, quText, ' ');
}

void GameDrawer::printRedQuestion(CubeID cube, Int2 TopLeft, String<16> quText)
{
	gVideo[cube].bg0.text(TopLeft, FontRed, quText, ' ');
}

void GameDrawer::printOperator(CubeID cube, Int2 TopLeft, int whichOp)
{
	gVideo[cube].bg0.image(TopLeft,OperImages[whichOp] );
}

void GameDrawer::printGreenOperator(CubeID cube, Int2 TopLeft, int whichOp)
{
	gVideo[cube].bg0.image(TopLeft,OperImagesGreen[whichOp] );
}

void GameDrawer::printRedOperator(CubeID cube, Int2 TopLeft, int whichOp)
{
	gVideo[cube].bg0.image(TopLeft,OperImagesRed[whichOp] );
}

void GameDrawer::doPanning(CubeID cube, Int2 Pan)
{
	gVideo[cube].bg0.setPanning(Pan);
}

void GameDrawer::paintGameOver(CubeID cube, int Score, int longestStreak)
{
	gVideo[cube].bg0.setPanning(vec(0,0));
	gVideo[cube].bg0.image(vec(0,0), GameOver);
	String<9> score;
	score << Score;
	String<9> lStreak;
	lStreak << longestStreak;
	gVideo[cube].bg0.text(vec(8,8),Font2, score, ' ');
	gVideo[cube].bg0.text(vec(8,12),Font2, lStreak, ' ');
}

void GameDrawer::GameDrawer::drawConnectCube(CubeID cube)
{
	gVideo[cube].bg0.image(vec(0,0),ConnectCube);
}

void GameDrawer::drawCountdown(CubeID cube, int CountdownSecs)
{
	//LOG("real cube id is = %d\n",(unsigned int)gVideo[cube].cube());
	gVideo[cube].bg0.image(vec(0,0),Countdown[CountdownSecs]);
}

void GameDrawer::drawOperatorBackground(CubeID cube)
{
	gVideo[cube].bg0.image(vec(0,0),Highlights[0]);
}

void GameDrawer::drawTimerBackground(CubeID cube)
{
	//LOG("real cube id is = %d\n",(unsigned int)gVideo[cube].cube());
	gVideo[cube].bg0.image(vec(0,0),TimerBackground);
}

void GameDrawer::drawQuestionerBackground(CubeID cube)
{
	gVideo[cube].bg0.image(vec(0,0),QuestionerBackground);
}

void GameDrawer::switchToBG0_BG1(CubeID cube)
{
	gVideo[cube].setMode(BG0_BG1);
}

void GameDrawer::switchToBG0(CubeID cube)
{
	gVideo[cube].setMode(BG0);
}

void GameDrawer::setBG1Mask(CubeID cube)
{
	gVideo[cube].bg1.setMask(BG1Mask::filled(vec(0,1),vec(16,1)));
}

void GameDrawer::drawTimeBar(CubeID cube)
{
	gVideo[cube].bg1.image(vec(0,1),Timebar);
}

void GameDrawer::drawTimeBarPartial(CubeID cube, Int2 TopLeft, int index)
{
	gVideo[cube].bg1.image(TopLeft,TimebarAnim[index]);
}

void GameDrawer::drawBlankTimeBar(CubeID cube, Int2 TopLeft)
{
	gVideo[cube].bg1.image(TopLeft, pixelsWhite);
}

void GameDrawer::paintBlack(CubeID cube)
{
	gVideo[cube].bg0.image(vec(0,0), BlackImage);
}

void GameDrawer::drawUpdatedResults(CubeID cube, int currStreak, int totalCorrect)
{
	if(!currStreak)
	{
		for(int i=10; i < 13; ++i)
		{
			gVideo[cube].bg0.image(vec(i,12), pixelsWhite);
			gVideo[cube].bg0.image(vec(i,13), pixelsWhite);
		}
	}
	String<9> score;
	score << totalCorrect;

	gVideo[cube].bg0.text(vec(10,8),Font, score, ' ');

	String<9> cStreak;
	cStreak << currStreak;
	gVideo[cube].bg0.text(vec(10,12),Font, cStreak, ' ');


}

void main()
{
	assetConfig.append(MainSlot, GameAssets);
	//assetConfig.append(MainSlot, MenuImageAssets);
	loader.init();

    Events::cubeConnect.set(&onConnect);
    Events::cubeDisconnect.set(&onDisconnect);
    Events::cubeTouch.set(&onTouch);

    Events::neighborAdd.set(&onNeighbourAdd);
    Events::neighborRemove.set(&onNeighbourRemove);

    for (CubeID cube : CubeSet::connected())
    {
    	//LOG("cube = %d\n",(int) cube);
    	onConnect(NULL, cube);
    }

    //LOG("I am about to create a game menu\n");

    MainMenu gameMenu(&gVideo[0]);

    while(1)
    {
    	//Do menu
    	Events::cubeTouch.unset();
	    Events::neighborAdd.unset();
	    Events::neighborRemove.unset();

        currMode = NULL;
        System::setCubeRange(1,CUBE_ALLOCATION);

    	int modeChosen = gameMenu.runMenu();

    	
    	//Do mode
    	switch(modeChosen)
    	{
    		case(0) :
    		{
    			//normal = Normal(&myGameDrawer);
    			//currMode = &normal;
    			break;
    		}
    		case(1) :
    		{
    			practise = Practise(&myGameDrawer);
                currMode = &practise;
    			break;
    		}
    		case(2) :
    		{
    			//Coop 2-Player
    			break;
    		}
    		case(3) :
    		{
    			//Coop 3-PLayer
    			break;
    		}
    		case(4) :
    		{
    			//Comp 2_player
    			break;
    		}
    		case(5) :
    		{
    			//Comp 3-Player
    			break;
    		}
    		case(6) :
    		{
    			//Stats
    			break;
    		}
    		case(7) :
    		{
    			//Rules
    			break;
    		}
    		default :
    		{
    			break;
    		}
    	}
    	
        Events::cubeTouch.set(&onTouch);
        Events::neighborAdd.set(&onNeighbourAdd);
        Events::neighborRemove.set(&onNeighbourRemove);

    	TimeStep ts;
		int exitLoop = 0;

		while(!exitLoop)
	    {
	    	//LOG("ts.delta() = %d\n",ts.delta().milliseconds());
	    	exitLoop = currMode->updateTime(ts.delta());

	    	ts.next();
	    	System::paint();
	    }
        //LOG("Finished running mode\n");
    }
}
