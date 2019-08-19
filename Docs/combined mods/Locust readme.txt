Locust of Chiron
================

Inspired by the SMAC unit - lots of flying, brain-eating worms! Animations, sounds and effects included... polycount not really applicable in the usual sense (since it only consists of effects). The attack animation is the swarm closing in, accompanied by a red wave of psi-energy - in case you want to use it as "normal" swarm of insects, use the locust_strike.kf from the "Locust_NoWave"-folder - it is the attack animation without the psi-wave.

Animations and particle texture both from scratch by Lord Tirian, made for the Planetfall mod.

The "Locust"-folder goes into assets\art\units\
The "Effects"-folder goes into assets\art\
The "Sounds"-folder goes into assets\

The recommended entry for the CIV4ArtDefines_Unit.xml is:
		<UnitArtInfo>
			<Type>ART_DEF_UNIT_LOCUST</Type>
			<Button>Art/Interface/Buttons/Units/LocustofChiron.dds</Button>
			<fScale>0.35</fScale>
			<fInterfaceScale>1.0</fInterfaceScale>
			<bActAsLand>0</bActAsLand>
			<bActAsAir>0</bActAsAir>
			<NIF>Art/Units/Locust/locust.nif</NIF>
			<KFM>Art/Units/Locust/locust.kfm</KFM>
			<SHADERNIF>Art/Units/Locust/locust.nif</SHADERNIF>
			<ShadowDef>
				<ShadowNIF>Art/Units/01_UnitShadows/UnitShadow.nif</ShadowNIF>
				<ShadowAttachNode>BIP Pelvis</ShadowAttachNode>
				<fShadowScale>2.1</fShadowScale>
			</ShadowDef>
			<fBattleDistance>0.25</fBattleDistance>
			<fRangedDeathTime>0.15</fRangedDeathTime>
			<bActAsRanged>0</bActAsRanged>
			<TrainSound>AS2D_UNIT_BUILD_UNIT</TrainSound>
			<AudioRunSounds>
				<AudioRunTypeLoop/>
				<AudioRunTypeEnd/>
			</AudioRunSounds>
		</UnitArtInfo>
		
This goes into the CIV4EffectInfos.xml:
		<EffectInfo>
			<Type>EFFECT_RED_PSIWAVE</Type>
			<Description>Red PSI-Wave</Description>
			<fScale>1.0</fScale>
			<fUpdateRate>1.0</fUpdateRate>
			<Path>Art/Effects/psiwave/fx_red_psiwave.nif</Path>
			<bIsProjectile>0</bIsProjectile>
		</EffectInfo>
		
These are the defines for the AudioDefines.xml:
		<SoundData>
			<SoundID>SND_LOCUST_ATTACK</SoundID>
			<Filename>Sounds/Units/LocustAttack</Filename>
			<LoadType>DYNAMIC_RES</LoadType>
			<bIsCompressed>1</bIsCompressed>
			<bInGeneric>1</bInGeneric>
		</SoundData>
		<SoundData>
			<SoundID>SND_LOCUST_DIE</SoundID>
			<Filename>Sounds/Units/LocustDie</Filename>
			<LoadType>DYNAMIC_RES</LoadType>
			<bIsCompressed>1</bIsCompressed>
			<bInGeneric>1</bInGeneric>
		</SoundData>
		<SoundData>
			<SoundID>SND_LOCUST_LOOP</SoundID>
			<Filename>Sounds/Units/LocustLoop</Filename>
			<LoadType>DYNAMIC_RES</LoadType>
			<bIsCompressed>1</bIsCompressed>
			<bInGeneric>1</bInGeneric>
		</SoundData>
		<SoundData>
			<SoundID>SND_LOCUST_NOISE</SoundID>
			<Filename>Sounds/Units/LocustNoise</Filename>
			<LoadType>DYNAMIC_RES</LoadType>
			<bIsCompressed>1</bIsCompressed>
			<bInGeneric>1</bInGeneric>
		</SoundData>
		
And, finally, this goes into Audio3DScripts.xml:
	<Script3DSound>
		<ScriptID>AS3D_LOCUST_ATTACK</ScriptID>
		<SoundID>SND_LOCUST_ATTACK</SoundID>
		<SoundType>GAME_SFX</SoundType>
		<iMinVolume>90</iMinVolume>
		<iMaxVolume>100</iMaxVolume>
		<iPitchChangeDown>0</iPitchChangeDown>
		<iPitchChangeUp>0</iPitchChangeUp>
		<bLooping>0</bLooping>
		<iMinTimeDelay>0</iMinTimeDelay>
		<iMaxTimeDelay>0</iMaxTimeDelay>
		<StartPosition>NONE</StartPosition>
		<EndPosition>NONE</EndPosition>
		<iMinVelocity>0</iMinVelocity>
		<iMaxVelocity>0</iMaxVelocity>
		<iMinDistanceFromListener>0</iMinDistanceFromListener>
		<iMaxDistanceFromListener>0</iMaxDistanceFromListener>
		<iMinDistanceForMaxVolume>500</iMinDistanceForMaxVolume>
		<iMaxDistanceForMaxVolume>1000</iMaxDistanceForMaxVolume>
		<iMinCutoffDistance>7000</iMinCutoffDistance>
		<iMaxCutoffDistance>7000</iMaxCutoffDistance>
		<bTaperForSoundtracks>0</bTaperForSoundtracks>
		<iLengthOfSound>0</iLengthOfSound>
		<fMinDryLevel>1.0</fMinDryLevel>
		<fMaxDryLevel>1.0</fMaxDryLevel>
		<fMinWetLevel>0.5</fMinWetLevel>
		<fMaxWetLevel>0.5</fMaxWetLevel>
		<iNotPlayPercent>0</iNotPlayPercent>
	</Script3DSound>
	<Script3DSound>
		<ScriptID>AS3D_LOCUST_DIE</ScriptID>
		<SoundID>SND_LOCUST_DIE</SoundID>
		<SoundType>GAME_SFX</SoundType>
		<iMinVolume>90</iMinVolume>
		<iMaxVolume>100</iMaxVolume>
		<iPitchChangeDown>0</iPitchChangeDown>
		<iPitchChangeUp>0</iPitchChangeUp>
		<bLooping>0</bLooping>
		<iMinTimeDelay>0</iMinTimeDelay>
		<iMaxTimeDelay>0</iMaxTimeDelay>
		<StartPosition>NONE</StartPosition>
		<EndPosition>NONE</EndPosition>
		<iMinVelocity>0</iMinVelocity>
		<iMaxVelocity>0</iMaxVelocity>
		<iMinDistanceFromListener>0</iMinDistanceFromListener>
		<iMaxDistanceFromListener>0</iMaxDistanceFromListener>
		<iMinDistanceForMaxVolume>500</iMinDistanceForMaxVolume>
		<iMaxDistanceForMaxVolume>1000</iMaxDistanceForMaxVolume>
		<iMinCutoffDistance>7000</iMinCutoffDistance>
		<iMaxCutoffDistance>7000</iMaxCutoffDistance>
		<bTaperForSoundtracks>0</bTaperForSoundtracks>
		<iLengthOfSound>0</iLengthOfSound>
		<fMinDryLevel>1.0</fMinDryLevel>
		<fMaxDryLevel>1.0</fMaxDryLevel>
		<fMinWetLevel>0.5</fMinWetLevel>
		<fMaxWetLevel>0.5</fMaxWetLevel>
		<iNotPlayPercent>0</iNotPlayPercent>
	</Script3DSound>
	<Script3DSound>
		<ScriptID>AS3D_LOCUST_LOOP</ScriptID>
		<SoundID>SND_LOCUST_LOOP</SoundID>
		<SoundType>GAME_SFX</SoundType>
		<iMinVolume>90</iMinVolume>
		<iMaxVolume>100</iMaxVolume>
		<iPitchChangeDown>0</iPitchChangeDown>
		<iPitchChangeUp>0</iPitchChangeUp>
		<bLooping>0</bLooping>
		<iMinTimeDelay>0</iMinTimeDelay>
		<iMaxTimeDelay>0</iMaxTimeDelay>
		<StartPosition>NONE</StartPosition>
		<EndPosition>NONE</EndPosition>
		<iMinVelocity>0</iMinVelocity>
		<iMaxVelocity>0</iMaxVelocity>
		<iMinDistanceFromListener>0</iMinDistanceFromListener>
		<iMaxDistanceFromListener>0</iMaxDistanceFromListener>
		<iMinDistanceForMaxVolume>500</iMinDistanceForMaxVolume>
		<iMaxDistanceForMaxVolume>1000</iMaxDistanceForMaxVolume>
		<iMinCutoffDistance>7000</iMinCutoffDistance>
		<iMaxCutoffDistance>7000</iMaxCutoffDistance>
		<bTaperForSoundtracks>0</bTaperForSoundtracks>
		<iLengthOfSound>0</iLengthOfSound>
		<fMinDryLevel>1.0</fMinDryLevel>
		<fMaxDryLevel>1.0</fMaxDryLevel>
		<fMinWetLevel>0.5</fMinWetLevel>
		<fMaxWetLevel>0.5</fMaxWetLevel>
		<iNotPlayPercent>0</iNotPlayPercent>
	</Script3DSound>
	<Script3DSound>
		<ScriptID>AS3D_LOCUST_NOISE</ScriptID>
		<SoundID>SND_LOCUST_NOISE</SoundID>
		<SoundType>GAME_SFX</SoundType>
		<iMinVolume>90</iMinVolume>
		<iMaxVolume>100</iMaxVolume>
		<iPitchChangeDown>0</iPitchChangeDown>
		<iPitchChangeUp>0</iPitchChangeUp>
		<bLooping>0</bLooping>
		<iMinTimeDelay>0</iMinTimeDelay>
		<iMaxTimeDelay>0</iMaxTimeDelay>
		<StartPosition>NONE</StartPosition>
		<EndPosition>NONE</EndPosition>
		<iMinVelocity>0</iMinVelocity>
		<iMaxVelocity>0</iMaxVelocity>
		<iMinDistanceFromListener>0</iMinDistanceFromListener>
		<iMaxDistanceFromListener>0</iMaxDistanceFromListener>
		<iMinDistanceForMaxVolume>500</iMinDistanceForMaxVolume>
		<iMaxDistanceForMaxVolume>1000</iMaxDistanceForMaxVolume>
		<iMinCutoffDistance>7000</iMinCutoffDistance>
		<iMaxCutoffDistance>7000</iMaxCutoffDistance>
		<bTaperForSoundtracks>0</bTaperForSoundtracks>
		<iLengthOfSound>0</iLengthOfSound>
		<fMinDryLevel>1.0</fMinDryLevel>
		<fMaxDryLevel>1.0</fMaxDryLevel>
		<fMinWetLevel>0.5</fMinWetLevel>
		<fMaxWetLevel>0.5</fMaxWetLevel>
		<iNotPlayPercent>0</iNotPlayPercent>
	</Script3DSound>