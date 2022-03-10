/*
	Created by: Hector Soto
	To test JALR.
*/

void Third()
{
	return;
}

void Second()
{
	Third();
	return;
}


void First()
{
	Second();
	return;
}


int main()
{
	First();
	return 0;
}

