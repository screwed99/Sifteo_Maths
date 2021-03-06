#include "Question.h"

Question::Question(GameDrawer* gameDrawer, CubeID cube, int yWritePosition)
{
	myGameDrawer = gameDrawer;
	myCube = cube;
	yPos = yWritePosition;
	firstOpWritten = 0;
	secondOpWritten = 0;
	answerTaken = 0;
	opsLocked = 0;
	myScore = 0;

	int questionReturn = QuestionList::getQuestion();

	questionArray[0] = questionReturn & 0x0000007f;
	questionArray[2] = (questionReturn & 0x00003f80) >> 7;
	questionArray[4] = (questionReturn & 0x001fc000) >> 14;
	questionArray[5] = (questionReturn & 0x0fe00000) >> 21;
	questionArray[1] = (questionReturn & 0x30000000) >> 28;
	questionArray[3] = (questionReturn & 0xc0000000) >> 30;

	int totalDigits = 5;
	xPosFirstOp = numDigits(questionArray[0]);
	totalDigits += xPosFirstOp;

	xPosSecondOp = numDigits(questionArray[2]);
	totalDigits += xPosSecondOp;
	totalDigits += numDigits(questionArray[4]);
	totalDigits += numDigits(questionArray[5]);

	extraSpace = 16 - totalDigits;
	extraSpace /= 2;

	xPosFirstOp += extraSpace;
	xPosSecondOp += xPosFirstOp + 2;

	String<16> quText;
	quText << questionArray[0] << "  " << questionArray[2] << "  "
		<< questionArray[4] << "=" << questionArray[5];
	myGameDrawer->printQuestion(myCube, vec(extraSpace,yPos), quText);

	//This is the 2 values more clearly presented, (wanted
	//to limit calls to totalDigits() method).
	//xPosFirstOp = extraSpace + numDigits(questionArray[0]);
	//xPosSecondOp = xPosFirstOp + 2 + numDigits(questionArray[2]);
}

Question::Question(GameDrawer* gameDrawer, CubeID cube, int yWritePosition, 
	int* question, int updateCorrect)
{
	myGameDrawer = gameDrawer;
	myCube = cube;
	yPos = yWritePosition;
	firstOpWritten = 0;
	secondOpWritten = 0;
	answerTaken = 0;
	opsLocked = 0;
	myScore = 0;

	int questionReturn = QuestionList::getQuestion();

	questionArray[0] = questionReturn & 0x0000007f;
	questionArray[2] = (questionReturn & 0x00003f80) >> 7;
	questionArray[4] = (questionReturn & 0x001fc000) >> 14;
	questionArray[5] = (questionReturn & 0x0fe00000) >> 21;
	questionArray[1] = (questionReturn & 0x30000000) >> 28;
	questionArray[3] = (questionReturn & 0xc0000000) >> 30;

	int totalDigits = 5;
	xPosFirstOp = numDigits(questionArray[0]);
	totalDigits += xPosFirstOp;

	xPosSecondOp = numDigits(questionArray[2]);
	totalDigits += xPosSecondOp;
	totalDigits += numDigits(questionArray[4]);
	totalDigits += numDigits(questionArray[5]);

	extraSpace = 16 - totalDigits;
	extraSpace /= 2;

	xPosFirstOp += extraSpace;
	xPosSecondOp += xPosFirstOp + 2;

	if(updateCorrect)
	{
		updateToCorrect();
	}
	else
	{
		String<16> quText;
		quText << questionArray[0] << "  " << questionArray[2] << "  "
			<< questionArray[4] << "=" << questionArray[5];
		myGameDrawer->printQuestion(myCube, vec(extraSpace,yPos), quText);
	}
}

//Used only when initialising lastQuestion to an empty question;
Question::Question(CubeID cube)
{
	yPos = 8;
	myCube = cube;
}

bool Question::equals(const Question& q) const
{
	return (questionArray[0] == q.questionArray[0] &&
		questionArray[2] == q.questionArray[2] &&
		questionArray[4] == q.questionArray[4] &&
		questionArray[5] == q.questionArray[5]);
}

int* Question::retrieveQuestion()
{
	return questionArray;
}

void Question::printOperator(int whichOp, int whichPos)
{
	if(!opsLocked)
	{
		if(whichPos == 1)
		{
			myGameDrawer->printOperator(myCube, vec(xPosFirstOp,yPos), whichOp);
			firstOpWritten = 1;
			opsChosen[0] = whichOp;
		}
		else
		{
			myGameDrawer->printOperator(myCube, vec(xPosSecondOp,yPos), whichOp);
			secondOpWritten = 1;
			opsChosen[1] = whichOp;
		}
		if(firstOpWritten && secondOpWritten)
		{
			questionSubmitted();// locks the operators using opsLocked
		}
	}
}

void Question::removeOperator(int whichPos)
{
	if(!opsLocked)
	{
		if(whichPos == 1)
		{
			myGameDrawer->printOperator(myCube, vec(xPosFirstOp,yPos), 4);
			firstOpWritten = 0;
		}
		else
		{
			myGameDrawer->printOperator(myCube, vec(xPosSecondOp,yPos), 4);
			secondOpWritten = 0;
		}
	}
}

void Question::clean()
{
	//LOG("myCube = %d\n",myCube);
	for(int i=0; i < 16; ++i)
	{
		myGameDrawer->printOperator(myCube, vec(i,yPos), 4);
	}
}

int Question::wasRight()
{
	return myScore;
}

int Question::answered()
{
	if(answerTaken)
	{
		answerTaken = 0;
		return 1;
	}
	return 0;
}

void Question::updateToCorrect()
{
	String<16> quText;
	quText << questionArray[0] << "  " << questionArray[2] << "  "
		<< questionArray[4] << "=" << questionArray[5];

	if(myScore)
	{
		myGameDrawer->printGreenQuestion(myCube, vec(extraSpace,yPos), quText);
		myGameDrawer->printGreenOperator(myCube, vec(xPosFirstOp,yPos), questionArray[1]);
		myGameDrawer->printGreenOperator(myCube, vec(xPosSecondOp,yPos), questionArray[3]);
	}
	else
	{
		myGameDrawer->printRedQuestion(myCube, vec(extraSpace,yPos), quText);
		myGameDrawer->printRedOperator(myCube, vec(xPosFirstOp,yPos), questionArray[1]);
		myGameDrawer->printRedOperator(myCube, vec(xPosSecondOp,yPos), questionArray[3]);
	}
}

int Question::numDigits(int number)
{
    int digits = 0;
    while (number)
    {
        number /= 10;
        digits++;
    }
    return digits;
}

void Question::questionSubmitted()
{
	answerTaken = 1;
	opsLocked = 1;
	if(opsChosen[0] == questionArray[1] && opsChosen[1] == questionArray[3])
	{
		myScore = 1;
		myGameDrawer->playCorrect();
	}
	else
	{
		myGameDrawer->playWrong();
	}
}