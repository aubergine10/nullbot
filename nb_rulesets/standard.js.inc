
/*
 * This file describes standard stats and strategies of 
 * the base (unmodded) game.
 * 
 * If you want to make an AI specially designed for your mod, start by
 * making a copy of this file and modifying it according to your mod's rules.
 * 
 * Then provide a personality to use the ruleset, similar to 
 * how nb_generic.[js|ai] is provided for this ruleset.
 * 
 * You may find some useful functions for working with these stats
 * in stats.js.inc .
 * 
 */

// a factor for figuring out how large things are in this ruleset,
// or simply a typical radius of a player's base
const baseScale = 20; 

// diameter of laser satellite splash/incendiary damage 
// for use in lassat.js.inc
const lassatSplash = 4; 

// this function is used for avoiding AI cheats that appear due to 
// being able to build droids before designing them
function iCanDesign() {
	if (difficulty === INSANE) // won't make INSANE much worse ...
		return true;
	return countFinishedStructList(structures.hqs) > 0;
}

const structures = {
	factories: [ "A0LightFactory", ],
	templateFactories: [ "A0CyborgFactory", ],
	vtolFactories: [ "A0VTolFactory1", ],
	labs: [ "A0ResearchFacility", ],
	gens: [ "A0PowerGenerator", ],
	hqs: [ "A0CommandCentre", ],
	vtolPads: [ "A0VtolPad", ],
	derricks: [ "A0ResourceExtractor", ],
	extras: [ "A0RepairCentre3", "A0Sat-linkCentre", "A0LasSatCommand", ],
	sensors: [ "Sys-SensoTower02", "Sys-CB-Tower01", "Sys-RadarDetector01", "Sys-SensoTowerWS", ],
};

const oilResources = [ "OilResource", ];

const powerUps = [ "OilDrum", "Crate" ];

// NOTE: you cannot use specific stats as bases, but only stattypes
// probably better make use of .name rather than of .stattype here?
const modules = [
	{ base: POWER_GEN, module: "A0PowMod1", count: 1, cost: MODULECOST.CHEAP },
	{ base: FACTORY, module: "A0FacMod1", count: 2, cost: MODULECOST.EXPENSIVE },
	{ base: VTOL_FACTORY, module: "A0FacMod1", count: 2, cost: MODULECOST.EXPENSIVE },
	{ base: RESEARCH_LAB, module: "A0ResearchModule1", count: 1, cost: MODULECOST.EXPENSIVE },
];

const targets = []
	.concat(structures.factories)
	.concat(structures.templateFactories)
	.concat(structures.vtolFactories)
	.concat(structures.extras)
;

const miscTargets = []
	.concat(structures.derricks)
;

const fundamentalResearch = [
	"R-Struc-PowerModuleMk1",
	"R-Struc-RprFac-Upgrade01",
	"R-Sys-Sensor-Tower02",
	"R-Struc-Power-Upgrade01c",
	"R-Sys-CBSensor-Tower01",
	"R-Struc-VTOLPad-Upgrade01",
	"R-Struc-Power-Upgrade03a",
	"R-Struc-VTOLPad-Upgrade03",
	"R-Sys-Autorepair-General",
	"R-Wpn-LasSat",
	"R-Struc-RprFac-Upgrade04",
	"R-Struc-VTOLPad-Upgrade06",
	"R-Struc-RprFac-Upgrade06",
];

const fastestResearch = [
	"R-Struc-Research-Upgrade09",
];

// body and propulsion arrays don't affect fixed template droids
const bodyStats = {
	kinetic: [
		{ res: "R-Vehicle-Body01", stat: "Body1REC", weight: WEIGHT.LIGHT, usage: BODYUSAGE.UNIVERSAL }, // viper
		{ res: "R-Vehicle-Body05", stat: "Body5REC", weight: WEIGHT.MEDIUM, usage: BODYUSAGE.COMBAT }, // cobra
		{ res: "R-Vehicle-Body11", stat: "Body11ABT", weight: WEIGHT.HEAVY, usage: BODYUSAGE.GROUND }, // python
		{ res: "R-Vehicle-Body02", stat: "Body2SUP", weight: WEIGHT.LIGHT, usage: BODYUSAGE.UNIVERSAL }, // leopard
		{ res: "R-Vehicle-Body06", stat: "Body6SUPP", weight: WEIGHT.MEDIUM, usage: BODYUSAGE.COMBAT }, // panther
		{ res: "R-Vehicle-Body09", stat: "Body9REC", weight: WEIGHT.HEAVY, usage: BODYUSAGE.GROUND }, // tiger
		{ res: "R-Vehicle-Body13", stat: "Body13SUP", weight: WEIGHT.HEAVY, usage: BODYUSAGE.GROUND }, // wyvern
		{ res: "R-Vehicle-Body14", stat: "Body14SUP", weight: WEIGHT.HEAVY, usage: BODYUSAGE.GROUND }, // dragon
	],
	thermal: [
		{ res: "R-Vehicle-Body04", stat: "Body4ABT", weight: WEIGHT.LIGHT, usage: BODYUSAGE.UNIVERSAL }, // bug
		{ res: "R-Vehicle-Body08", stat: "Body8MBT", weight: WEIGHT.HEAVY, usage: BODYUSAGE.COMBAT }, // scorpion
		{ res: "R-Vehicle-Body12", stat: "Body12SUP", weight: WEIGHT.HEAVY, usage: BODYUSAGE.GROUND }, // mantis
		{ res: "R-Vehicle-Body03", stat: "Body3MBT", weight: WEIGHT.MEDIUM, usage: BODYUSAGE.UNIVERSAL }, // retaliation
		{ res: "R-Vehicle-Body07", stat: "Body7ABT", weight: WEIGHT.HEAVY, usage: BODYUSAGE.COMBAT }, // retribution
		{ res: "R-Vehicle-Body10", stat: "Body10MBT", weight: WEIGHT.HEAVY, usage: BODYUSAGE.GROUND }, // vengeance
	],
}

const classResearch = {
	kinetic: {
		tank: [
			"R-Vehicle-Metals09",
		],
		borg: [
			"R-Cyborg-Metals09",
		],
		defs: [
			"R-Defense-WallUpgrade12",
			"R-Struc-Materials09",
		],
	},
	thermal: {
		tank: [
			"R-Vehicle-Armor-Heat09",
		],
		borg: [
			"R-Cyborg-Armor-Heat09",
		],
		defs: [
			"R-Defense-WallUpgrade12",
			"R-Struc-Materials09",
		],
	},
}

// NOTE: Please don't put hover propulsion into the ground list, etc.! 
// Adaptation code relies on that for discovering map topology.
// Ground propulsions need to be ground only, hover propulsions shouldn't
// be able to cross cliffs, but should be able to cross seas, etc. 
const propulsionStats = {
	ground: [
		{ res: "R-Vehicle-Prop-Wheels", stat: "wheeled01" },
		{ res: "R-Vehicle-Prop-Halftracks", stat: "HalfTrack" },
		{ res: "R-Vehicle-Prop-Tracks", stat: "tracked01" },
	],
	hover: [
		{ res: "R-Vehicle-Prop-Hover", stat: "hover01" },
	],
	vtol: [
		{ res: "R-Vehicle-Prop-VTOL", stat: "V-Tol" },
	],
}

const truckTurrets = [
	"Spade1Mk1",
];

const truckTemplates = [
	{ body: "Cyb-Bod-ComEng", prop: "CyborgLegs", weapons: [ "CyborgSpade", ] } // engineer
];

// Unlike bodies and propulsions, weapon lines don't have any specific meaning.
// You can make as many weapon lines as you want for your ruleset.
const weaponStats = {
	machineguns: {
		role: ROLE.AP,
		chatalias: "mg",
		weapons: [
			{ res: "R-Wpn-MG1Mk1", stat: "MG1Mk1", weight: WEIGHT.ULTRALIGHT }, // mg
			{ res: "R-Wpn-MG2Mk1", stat: "MG2Mk1", weight: WEIGHT.LIGHT }, // tmg
			{ res: "R-Wpn-MG3Mk1", stat: "MG3Mk1", weight: WEIGHT.MEDIUM }, // hmg
			{ res: "R-Wpn-MG4", stat: "MG4ROTARYMk1", weight: WEIGHT.MEDIUM }, // ag
			{ res: "R-Wpn-MG5", stat: "MG5TWINROTARY", weight: WEIGHT.MEDIUM }, // tag
		],
		vtols: [
			{ res: "R-Wpn-MG3Mk1", stat: "MG3-VTOL", weight: WEIGHT.ULTRALIGHT }, // vtol hmg
			{ res: "R-Wpn-MG4", stat: "MG4ROTARY-VTOL", weight: WEIGHT.MEDIUM }, // vtol ag
		],
		defenses: [
			{ res: "R-Defense-Tower01", stat: "GuardTower1", defrole: DEFROLE.GATEWAY }, // hmg tower
			{ res: "R-Defense-Pillbox01", stat: "PillBox1", defrole: DEFROLE.STANDALONE }, // hmg bunker
			{ res: "R-Defense-WallTower01", stat: "WallTower01", defrole: DEFROLE.GATEWAY }, // hmg hardpoint
			{ res: "R-Defense-RotMG", stat: "Pillbox-RotMG", defrole: DEFROLE.STANDALONE }, // ag bunker
			{ res: "R-Defense-Wall-RotMg", stat: "Wall-RotMg", defrole: DEFROLE.GATEWAY }, // ag hardpoint
			{ res: "R-Defense-WallTower-TwinAGun", stat: "WallTower-TwinAssaultGun", defrole: DEFROLE.GATEWAY }, // tag hardpoint
		],
		templates: [
			{ res: "R-Wpn-MG1Mk1", body: "CyborgChain1Ground", prop: "CyborgLegs", weapons: [ "CyborgChaingun", ] }, // mg cyborg
			{ res: "R-Wpn-MG4", body: "CybRotMgGrd", prop: "CyborgLegs", weapons: [ "CyborgRotMG", ] }, // ag cyborg
		],
		extras: [
			"R-Wpn-MG-Damage08",
		],
	},
	flamers: {
		role: ROLE.AP,
		chatalias: "fl",
		weapons: [
			{ res: "R-Wpn-Flamer01Mk1", stat: "Flame1Mk1", weight: WEIGHT.LIGHT }, // flamer
			{ res: "R-Wpn-Flame2", stat: "Flame2", weight: WEIGHT.HEAVY }, // inferno
			{ res: "R-Wpn-Plasmite-Flamer", stat: "PlasmiteFlamer", weight: WEIGHT.HEAVY }, // plasmite
		],
		vtols: [],
		defenses: [
			{ res: "R-Defense-Pillbox05", stat: "PillBox5", defrole: DEFROLE.GATEWAY }, // flamer bunker
			{ res: "R-Defense-HvyFlamer", stat: "Tower-Projector", defrole: DEFROLE.GATEWAY }, // inferno bunker
			{ res: "R-Defense-PlasmiteFlamer", stat: "Plasmite-flamer-bunker", defrole: DEFROLE.GATEWAY }, // plasmite bunker
		],
		templates: [
			{ res: "R-Wpn-Flamer01Mk1", body: "CyborgFlamerGrd", prop: "CyborgLegs", weapons: [ "CyborgFlamer01", ] }, // flamer cyborg
			{ res: "R-Wpn-Flame2", body: "Cyb-Bod-Thermite", prop: "CyborgLegs", weapons: [ "Cyb-Wpn-Thermite", ] }, // flamer cyborg
		],
		extras: [
			"R-Wpn-Flamer-ROF03",
			"R-Wpn-Flamer-Damage09",
		],
	},
	cannons: {
		role: ROLE.AT,
		chatalias: "cn",
		weapons: [
			{ res: "R-Wpn-Cannon1Mk1", stat: "Cannon1Mk1", weight: WEIGHT.LIGHT }, // lc
			{ res: "R-Wpn-Cannon2Mk1", stat: "Cannon2A-TMk1", weight: WEIGHT.HEAVY }, // mc
			{ res: "R-Wpn-Cannon4AMk1", stat: "Cannon4AUTOMk1", weight: WEIGHT.HEAVY }, // hpv
			{ res: "R-Wpn-Cannon5", stat: "Cannon5VulcanMk1", weight: WEIGHT.HEAVY }, // ac
			{ res: "R-Wpn-Cannon6TwinAslt", stat: "Cannon6TwinAslt", weight: WEIGHT.ULTRAHEAVY }, // tac
			{ res: "R-Wpn-Cannon3Mk1", stat: "Cannon375mmMk1", weight: WEIGHT.ULTRAHEAVY }, // hc
			{ res: "R-Wpn-RailGun01", stat: "RailGun1Mk1", weight: WEIGHT.LIGHT }, // needle
			{ res: "R-Wpn-RailGun02", stat: "RailGun2Mk1", weight: WEIGHT.HEAVY }, // rail
			{ res: "R-Wpn-RailGun03", stat: "RailGun3Mk1", weight: WEIGHT.ULTRAHEAVY }, // gauss
		],
		vtols: [
			{ res: "R-Wpn-Cannon1Mk1", stat: "Cannon1-VTOL", weight: WEIGHT.LIGHT }, // lc
			{ res: "R-Wpn-Cannon4AMk1", stat: "Cannon4AUTO-VTOL", weight: WEIGHT.HEAVY }, // hpv
			{ res: "R-Wpn-Cannon5", stat: "Cannon5Vulcan-VTOL", weight: WEIGHT.HEAVY }, // ac
			{ res: "R-Wpn-RailGun01", stat: "RailGun1-VTOL", weight: WEIGHT.LIGHT }, // needle
			{ res: "R-Wpn-RailGun02", stat: "RailGun2-VTOL", weight: WEIGHT.HEAVY }, // rail
		],
		defenses: [
			{ res: "R-Defense-Pillbox04", stat: "PillBox4", defrole: DEFROLE.STANDALONE }, // lc bunker
			{ res: "R-Defense-WallTower02", stat: "WallTower02", defrole: DEFROLE.GATEWAY }, // lc hard
			{ res: "R-Defense-WallTower03", stat: "WallTower03", defrole: DEFROLE.GATEWAY }, // mc hard
			{ res: "R-Defense-Emplacement-HPVcannon", stat: "Emplacement-HPVcannon", defrole: DEFROLE.STANDALONE }, // hpv empl
			{ res: "R-Defense-WallTower-HPVcannon", stat: "WallTower-HPVcannon", defrole: DEFROLE.GATEWAY }, // hpv hard
			{ res: "R-Defense-Wall-VulcanCan", stat: "Wall-VulcanCan", defrole: DEFROLE.GATEWAY }, // ac hard
			{ res: "R-Defense-Cannon6", stat: "PillBox-Cannon6", defrole: DEFROLE.STANDALONE }, // tac bunker
			{ res: "R-Defense-WallTower04", stat: "WallTower04", defrole: DEFROLE.GATEWAY }, // hc hard
			{ res: "R-Defense-Super-Cannon", stat: "X-Super-Cannon", defrole: DEFROLE.FORTRESS }, // cannon fort
			{ res: "R-Defense-GuardTower-Rail1", stat: "GuardTower-Rail1", defrole: DEFROLE.STANDALONE }, // needle tower
			{ res: "R-Defense-Rail2", stat: "Emplacement-Rail2", defrole: DEFROLE.STANDALONE }, // rail empl
			{ res: "R-Defense-WallTower-Rail2", stat: "WallTower-Rail2", defrole: DEFROLE.GATEWAY }, // rail hard
			{ res: "R-Defense-Rail3", stat: "Emplacement-Rail3", defrole: DEFROLE.STANDALONE }, // gauss empl
			{ res: "R-Defense-WallTower-Rail3", stat: "WallTower-Rail3", defrole: DEFROLE.GATEWAY }, // gauss hard
			{ res: "R-Defense-MassDriver", stat: "X-Super-MassDriver", defrole: DEFROLE.FORTRESS }, // mass driver fort
		],
		templates: [
			{ res: "R-Wpn-Cannon1Mk1", body: "CyborgCannonGrd", prop: "CyborgLegs", weapons: [ "CyborgCannon", ] }, // lc borg
			{ res: "R-Cyborg-Hvywpn-Mcannon", body: "Cyb-Hvybod-Mcannon", prop: "CyborgLegs", weapons: [ "Cyb-Hvywpn-Mcannon", ] }, // mc super
			{ res: "R-Cyborg-Hvywpn-HPV", body: "Cyb-Hvybod-HPV", prop: "CyborgLegs", weapons: [ "Cyb-Hvywpn-HPV", ] }, // hpv super
			{ res: "R-Cyborg-Hvywpn-Acannon", body: "Cyb-Hvybod-Acannon", prop: "CyborgLegs", weapons: [ "Cyb-Hvywpn-Acannon", ] }, // ac super
			{ res: "R-Wpn-RailGun01", body: "Cyb-Bod-Rail1", prop: "CyborgLegs", weapons: [ "Cyb-Wpn-Rail1", ] }, // needle borg
			{ res: "R-Cyborg-Hvywpn-RailGunner", body: "Cyb-Hvybod-RailGunner", prop: "CyborgLegs", weapons: [ "Cyb-Hvywpn-RailGunner", ] }, // rail super
		],
		extras: [
			"R-Wpn-Cannon-ROF06",
			"R-Vehicle-Engine09", // cannons are heeeeavy
			"R-Wpn-Rail-Damage03", // sure it's required by gauss, but what if our AI uses only cyborgs and vtols?
			"R-Wpn-Rail-ROF03",
		],
	},
	cannons_AA: {
		role: ROLE.AA,
		chatalias: "ca",
		weapons: [
			{ res: "R-Wpn-AAGun02", stat: "AAGun2Mk1", weight: WEIGHT.HEAVY },
		],
		vtols: [],
		defenses: [
			{ res: "R-Defense-AASite-QuadBof", stat: "AASite-QuadBof", defrole: DEFROLE.STANDALONE },
			{ res: "R-Defense-WallTower-DoubleAAgun", stat: "WallTower-DoubleAAGun", defrole: DEFROLE.GATEWAY },
		],
		templates: [],
		extras: [],
	},
	mortars: {
		role: ROLE.AS,
		chatalias: "mo",
		weapons: [
			{ res: "R-Wpn-Mortar01Lt", stat: "Mortar1Mk1", weight: WEIGHT.HEAVY },
			{ res: "R-Wpn-Mortar02Hvy", stat: "Mortar2Mk1", weight: WEIGHT.HEAVY },
			{ res: "R-Wpn-Mortar3", stat: "Mortar3ROTARYMk1", weight: WEIGHT.HEAVY },
			{ res: "R-Wpn-HowitzerMk1", stat: "Howitzer105Mk1", weight: WEIGHT.ULTRAHEAVY },
			{ res: "R-Wpn-Howitzer03-Rot", stat: "Howitzer03-Rot", weight: WEIGHT.ULTRAHEAVY },
			{ res: "R-Wpn-HvyHowitzer", stat: "Howitzer150Mk1", weight: WEIGHT.ULTRAHEAVY },
		],
		vtols: [
			{ res: "R-Wpn-Bomb01", stat: "Bomb1-VTOL-LtHE", weight: WEIGHT.LIGHT },
			{ res: "R-Wpn-Bomb02", stat: "Bomb2-VTOL-HvHE", weight: WEIGHT.HEAVY },
		],
		defenses: [
			{ res: "R-Defense-MortarPit", stat: "Emplacement-MortarPit01", defrole: DEFROLE.ARTY },
			{ res: "R-Defense-HvyMor", stat: "Emplacement-MortarPit02", defrole: DEFROLE.ARTY },
			{ res: "R-Defense-RotMor", stat: "Emplacement-RotMor", defrole: DEFROLE.ARTY },
			{ res: "R-Defense-Howitzer", stat: "Emplacement-Howitzer105", defrole: DEFROLE.ARTY },
			{ res: "R-Defense-RotHow", stat: "Emplacement-RotHow", defrole: DEFROLE.ARTY },
			{ res: "R-Defense-HvyHowitzer", stat: "Emplacement-Howitzer150", defrole: DEFROLE.ARTY },
		],
		templates: [
			{ res: "R-Wpn-Mortar01Lt", body: "Cyb-Bod-Grenade", prop: "CyborgLegs", weapons: [ "Cyb-Wpn-Grenade", ] },
		],
		extras: [
			"R-Wpn-Bomb-Accuracy03",
			"R-Wpn-Howitzer-Damage06",
			"R-Wpn-Howitzer-ROF04",
			"R-Wpn-Howitzer-Accuracy03",
		],
	},
	fireMortars: {
		role: ROLE.AS,
		chatalias: "fm", 
		weapons: [
			{ res: "R-Wpn-Mortar-Incenediary", stat: "Mortar-Incenediary", weight: WEIGHT.HEAVY },
			{ res: "R-Wpn-Howitzer-Incenediary", stat: "Howitzer-Incenediary", weight: WEIGHT.ULTRAHEAVY },
		],
		vtols: [
			{ res: "R-Wpn-Bomb03", stat: "Bomb3-VTOL-LtINC", weight: WEIGHT.LIGHT },
			{ res: "R-Wpn-Bomb04", stat: "Bomb4-VTOL-HvyINC", weight: WEIGHT.HEAVY },
			{ res: "R-Wpn-Bomb05", stat: "Bomb5-VTOL-Plasmite", weight: WEIGHT.HEAVY },
		],
		defenses: [
			{ res: "R-Defense-MortarPit-Incenediary", stat: "Emplacement-MortarPit-Incenediary", defrole: DEFROLE.ARTY },
			{ res: "R-Defense-Howitzer-Incenediary", stat: "Emplacement-Howitzer-Incenediary", defrole: DEFROLE.ARTY },
		],
		templates: [],
		extras: [
			"R-Wpn-Bomb-Accuracy03",
			"R-Wpn-Howitzer-Damage06",
			"R-Wpn-Howitzer-ROF04",
			"R-Wpn-Howitzer-Accuracy03",
		],
	},
	rockets_AT: {
		role: ROLE.AT,
		chatalias: "rx",
		weapons: [
			{ res: "R-Wpn-Rocket05-MiniPod", stat: "Rocket-Pod", weight: WEIGHT.LIGHT }, // pod
			{ res: "R-Wpn-Rocket01-LtAT", stat: "Rocket-LtA-T", weight: WEIGHT.LIGHT }, // lancer
			{ res: "R-Wpn-Rocket07-Tank-Killer", stat: "Rocket-HvyA-T", weight: WEIGHT.LIGHT }, // tk
			{ res: "R-Wpn-Missile2A-T", stat: "Missile-A-T", weight: WEIGHT.LIGHT }, // scourge
		],
		vtols: [
			{ res: "R-Wpn-Rocket05-MiniPod", stat: "Rocket-VTOL-Pod", weight: WEIGHT.ULTRALIGHT }, // pod
			{ res: "R-Wpn-Rocket01-LtAT", stat: "Rocket-VTOL-LtA-T", weight: WEIGHT.LIGHT }, // lancer
			{ res: "R-Wpn-Rocket07-Tank-Killer", stat: "Rocket-VTOL-HvyA-T", weight: WEIGHT.LIGHT }, // tk
			{ res: "R-Wpn-Missile2A-T", stat: "Missile-VTOL-AT", weight: WEIGHT.LIGHT }, // scourge
		],
		defenses: [
			{ res: "R-Defense-Tower06", stat: "GuardTower6", defrole: DEFROLE.STANDALONE }, // pod tower
			{ res: "R-Defense-Pillbox06", stat: "GuardTower5", defrole: DEFROLE.STANDALONE }, // lancer tower
			{ res: "R-Defense-WallTower06", stat: "WallTower06", defrole: DEFROLE.GATEWAY }, // lancer hardpoint
			{ res: "R-Defense-HvyA-Trocket", stat: "Emplacement-HvyATrocket", defrole: DEFROLE.STANDALONE }, // tk emplacement
			{ res: "R-Defense-WallTower-HvyA-Trocket", stat: "WallTower-HvATrocket", defrole: DEFROLE.GATEWAY }, // tk hardpoint
			{ res: "R-Defense-Super-Rocket", stat: "X-Super-Rocket", defrole: DEFROLE.FORTRESS }, // rocket bastion
			{ res: "R-Defense-GuardTower-ATMiss", stat: "GuardTower-ATMiss", defrole: DEFROLE.STANDALONE }, // scourge tower
			{ res: "R-Defense-WallTower-A-Tmiss", stat: "WallTower-Atmiss", defrole: DEFROLE.GATEWAY }, // scourge hardpoint
			{ res: "R-Defense-Super-Missile", stat: "X-Super-Missile", defrole: DEFROLE.FORTRESS }, // missile fortress
		],
		templates: [
			{ res: "R-Wpn-Rocket01-LtAT", body: "CyborgRkt1Ground", prop: "CyborgLegs", weapons: [ "CyborgRocket", ] }, // lancer borg
			{ res: "R-Cyborg-Hvywpn-TK", body: "Cyb-Hvybod-TK", prop: "CyborgLegs", weapons: [ "Cyb-Hvywpn-TK", ] }, // tk super
			{ res: "R-Wpn-Missile2A-T", body: "Cyb-Bod-Atmiss", prop: "CyborgLegs", weapons: [ "Cyb-Wpn-Atmiss", ] }, // scourge borg
			{ res: "R-Cyborg-Hvywpn-A-T", body: "Cyb-Hvybod-A-T", prop: "CyborgLegs", weapons: [ "Cyb-Hvywpn-A-T", ] }, // scourge super
		],
		extras: [
			"R-Wpn-Rocket-ROF03",
			"R-Wpn-Missile-Damage03",
			"R-Wpn-Missile-ROF03",
			"R-Wpn-Missile-Accuracy02",
		],
	},
	rockets_AS: {
		role: ROLE.AS,
		chatalias: "rxas",
		weapons: [
			{ res: "R-Wpn-Rocket02-MRL", stat: "Rocket-MRL", weight: WEIGHT.LIGHT }, // mra
			{ res: "R-Wpn-Rocket03-HvAT", stat: "Rocket-BB", weight: WEIGHT.MEDIUM }, // bb
			{ res: "R-Wpn-Rocket06-IDF", stat: "Rocket-IDF", weight: WEIGHT.ULTRAHEAVY }, // ripple
			{ res: "R-Wpn-HvArtMissile", stat: "Missile-HvyArt", weight: WEIGHT.ULTRAHEAVY }, // archie
		],
		vtols: [
			{ res: "R-Wpn-Rocket03-HvAT", stat: "Rocket-VTOL-BB", weight: WEIGHT.LIGHT }, // bb
		],
		defenses: [
			{ res: "R-Defense-MRL", stat: "Emplacement-MRL-pit", defrole: DEFROLE.STANDALONE }, // mra
			{ res: "R-Defense-IDFRocket", stat: "Emplacement-Rocket06-IDF", defrole: DEFROLE.ARTY }, // ripple
			{ res: "R-Defense-HvyArtMissile", stat: "Emplacement-HvART-pit", defrole: DEFROLE.ARTY }, // archie
		],
		templates: [],
		extras: [],
	},
	rockets_AA: {
		role: ROLE.AA,
		chatalias: "rxaa",
		weapons: [
			{ res: "R-Wpn-Sunburst", stat: "Rocket-Sunburst", weight: WEIGHT.LIGHT }, // sunburst
			{ res: "R-Wpn-Missile-LtSAM", stat: "Missile-LtSAM", weight: WEIGHT.LIGHT }, // avenger
			{ res: "R-Wpn-Missile-HvSAM", stat: "Missile-HvySAM", weight: WEIGHT.HEAVY }, // vindicator
		],
		vtols: [
			{ res: "R-Wpn-Sunburst", stat: "Rocket-VTOL-Sunburst", weight: WEIGHT.LIGHT }, // sunburst a2a
		],
		defenses: [
			{ res: "R-Defense-Sunburst", stat: "P0-AASite-Sunburst", defrole: DEFROLE.STANDALONE }, // sunburst
			{ res: "R-Defense-SamSite1", stat: "P0-AASite-SAM1", defrole: DEFROLE.STANDALONE }, // avenger
			{ res: "R-Defense-WallTower-SamSite", stat: "WallTower-SamSite", defrole: DEFROLE.GATEWAY }, // avenger
			{ res: "R-Defense-SamSite2", stat: "P0-AASite-SAM2", defrole: DEFROLE.STANDALONE }, // vindicator
			{ res: "R-Defense-WallTower-SamHvy", stat: "WallTower-SamHvy", defrole: DEFROLE.GATEWAY }, // vindicator hardpoint
		],
		templates: [],
		extras: [],
	},
	rockets_late_AP: {
		role: ROLE.AP,
		chatalias: "sr",
		weapons: [
			{ res: "R-Wpn-MdArtMissile", stat: "Missile-MdArt", weight: WEIGHT.HEAVY }, // seraph
		],
		vtols: [],
		defenses: [
			{ res: "R-Defense-MdArtMissile", stat: "Emplacement-MdART-pit", defrole: DEFROLE.STANDALONE }, // seraph
		],
		templates: [],
		extras: [],
	},
	lasers: {
		role: ROLE.AP,
		chatalias: "ls",
		weapons: [
			{ res: "R-Wpn-Laser01", stat: "Laser3BEAMMk1", weight: WEIGHT.ULTRALIGHT }, // flash
			{ res: "R-Wpn-Laser02", stat: "Laser2PULSEMk1", weight: WEIGHT.HEAVY }, // pulse
			{ res: "R-Wpn-HvyLaser", stat: "HeavyLaser", weight: WEIGHT.ULTRAHEAVY }, // hvy laser
		],
		vtols: [
			{ res: "R-Wpn-Laser01", stat: "Laser3BEAM-VTOL", weight: WEIGHT.ULTRALIGHT }, // flash
			{ res: "R-Wpn-Laser02", stat: "Laser2PULSE-VTOL", weight: WEIGHT.HEAVY }, // pulse
			{ res: "R-Wpn-HvyLaser", stat: "HeavyLaser-VTOL", weight: WEIGHT.HEAVY }, // hvy laser
		],
		defenses: [
			{ res: "R-Defense-PrisLas", stat: "Emplacement-PrisLas", defrole: DEFROLE.STANDALONE }, // flash empl
			{ res: "R-Defense-PulseLas", stat: "GuardTower-BeamLas", defrole: DEFROLE.STANDALONE }, // pulse tower
			{ res: "R-Defense-WallTower-PulseLas", stat: "WallTower-PulseLas", defrole: DEFROLE.GATEWAY }, // pulse hard
			{ res: "R-Defense-HeavyLas", stat: "Emplacement-HeavyLaser", defrole: DEFROLE.STANDALONE }, // hvy empl
		],
		templates: [
			{ res: "R-Wpn-Laser01", body: "Cyb-Bod-Las1", prop: "CyborgLegs", weapons: [ "Cyb-Wpn-Laser", ] }, // flash borg
			{ res: "Cyb-Hvy-PulseLsr", body: "Cyb-Hvybod-PulseLsr", prop: "CyborgLegs", weapons: [ "Cyb-Hvywpn-PulseLsr", ] }, // pulse super
		],
		extras: [],
	},
	useless_AT: {
		role: ROLE.AT,
		chatalias: "useless_AT",
		weapons: [],
		vtols: [],
		defenses: [],
		templates: [],
		extras: [],
	},
	useless_AP: {
		role: ROLE.AP,
		chatalias: "useless_AP",
		weapons: [
			{ res: "", stat: "MG3-Pillbox", weight: 0 }, // mg bunker dedicated weapon
		],
		vtols: [],
		defenses: [],
		templates: [],
		extras: [],
	},
	useless_AS: {
		role: ROLE.AS,
		chatalias: "useless_AS",
		weapons: [],
		vtols: [],
		defenses: [],
		templates: [],
		exxtras: [],
	},
	useless_AA: {
		role: ROLE.AA,
		chatalias: "useless_AA",
		weapons: [],
		vtols: [],
		defenses: [],
		templates: [],
		extras: [],
	},
};
