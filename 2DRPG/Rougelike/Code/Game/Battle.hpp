#pragma once
//////////////////////////////////////////////////////////////////////////
class Pawn;
class Tile;
//////////////////////////////////////////////////////////////////////////
namespace Battle
{
	const double PHYBASEDMG1 = 32; //Original = 32
	const double PHYBASEDMG2 = 16; //Original = 16
	
	const double MAGBASEDMG1 = 6; //Original = 6
	const double MAGBASEDMG2 = 4; //Original = 4
	
	const double DEFNUM1 = 280.4; //Original = 280.4
	const double DEFNUM2 = 110;	//Original = 110
	const double DEFNUM3 = 16;    //Original = 16
	
	const double REDDMGNUM = 730; //Original = 730
	
	const double FINALDMG1 = 730; //Original = 730
	const double FINALDMG2 = 51; //Original = 51
	const double FINALDMG3 = 11; //Original = 11
	const double FINALDMG4 = 10; //Original = 10
	//****************************************************************************************************
	// Formula's
	// These Formula's are based off of the battle system for Final Fantasy X


	// Stat is Attack stat + enhancements, DmCon is Damage constant of Physical Attack (Basic Attack DmCon = 16).
	double PhysicalBaseDmg(double Stat, double DmCon);
	

	// Stat is Magic stat + enhancements, DmCon is Damage constant of Magical Attack, mid power magic hasDmCon of 24
	double MagicalBaseDmg(double Stat, double DmCon);
	

	// converts Defensive stat to an int between 0 and 730
	double DefNum(double Def);
	

	double ReducedDamage(double Dmg, double Def);
	

	double FinalDamage(double BaseDmg, double Def);
	



	//****************************************************************************************************


	int CalcDamage(int attackType, double AttStat, double DmCon, double Def);
	

	//////////////////////////////////////////////////////////////////////////
	
	void BasicFight(Pawn& attacker, Tile& tileTarget);
}