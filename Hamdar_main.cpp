// WallstenCalibration.cpp : Defines the entry point for the console application.

// Calibration Using NGSIM Trajectory Data

//#include "stdafx.h"
#include <vector>
#include <ctype.h>
#include <math.h>
//#include <stdio.h>

//#include <conio.h>
#include <stdlib.h>
//#include <dos.h>
#include <string.h>

#include <time.h>
#include <iostream>
using namespace std;

#define _TCHAR char
//****************************Definition of Functions/Structs/Vectors/Variables*******************************

///////////////////////////////////////////////////////////
// This file deals with the reading of numbers/lines from files.
int g_Readnum(FILE* f)
// read an integer from the current pointer of the file, skip all spaces
{
    char ch, buf[ 32 ];
    int i = 0;
    int flag = 1;
    /* returns -1 if end of file is reached */
    
    while(true)
    {
        ch = getc( f );
        if( ch == EOF ) return -1;
        if (isdigit(ch))
            break;
        if (ch == '-')
            flag = -1;
        else
            flag = 1;
    };
    if( ch == EOF ) return -1;
    while( isdigit( ch )) {
        buf[ i++ ] = ch;
        ch = fgetc( f );
    }
    buf[ i ] = 0;
    
    
    return atoi( buf ) * flag;
    
}

double GammaGenerate (double scale, double shape)
{
    double Storage;
    Storage = 0.0;
    
    for (int i = 0; i < (int)shape; i++)
        Storage = Storage + log ( ( (rand()%1000) / 1000.0 ) );
    
    return -scale * Storage;
}

double g_Readdouble(FILE *f)
/* read a floating point number from the current pointer of the file,
 skip all spaces
 */
{
    char ch, buf[ 32 ];
    int i = 0;
    int flag = 1;
    
    /* returns -1 if end of file is reached */
    
    while(true)
    {
        ch = getc( f );
        if( ch == EOF ) return -1;
        if (isdigit(ch))
            break;
        
        if (ch == '-')
            flag = -1;
        else
            flag = 1;
        
    };
    if( ch == EOF ) return -1;
    while( isdigit( ch ) || ch == '.' ) {
        buf[ i++ ] = ch;
        ch = fgetc( f );
    }
    buf[ i ] = 0;
    
    /* atof function converts a character string (char *) into a doubleing
     pointer equivalent, and if the string is not a floting point number,
     a zero will be return.
     */
    
    return atof( buf ) * flag;
}

//_____________________________________________________________________________________
//Added by Samer Hamdar: 6 October 2008
struct VehDurationDataSUB
{
    int CorrespondingFrameID;       //Frame ID Corresponding to the Duration, Added by Samer Hamdar, March 15, 2007
    int DurationID;                    //Duration ID: Same as for the "Mother Duration"
    int DurationTimeSUB;            //Duration Time (Multiple of 0.1 seconds)
    
    //Added by Samer Hamdar, March 13, 2007
    int EpisodeType;                //1- Exit by Changing Lanes from Car-Following Episode
    //2- Exit by Entering Free-Flow Episode from Car-Following Episode
    //3- Exit by Changing Lanes from Free-Flow Episode
    //4- Exit by Entering Car-Following Episode from Free-Flow Episode
    //5- Exit by Leaving Area from Car-Following Episode
    //3- Exit by Leaving Area from Free-Flow Episode
    int VehicleType;                // 1: Motorcycle 2: Auto 3: Truck (temporary)
    double DriverSpeed;             //At the end of the duration (temporary)
    double RelativeSpeedLeader1;    //At the end of the duration (temporary) (999 if no leader 1)
    double DistanceHeadwayLeader1;  //At the end of the duration (temporary) (9999 if no leader 1) (front to front)
    double RelativeSpeedFollower1;  //At the end of the duration (temporary) (999 if no follower 1)
    double DistanceHeadwayFollower1;//At the end of the duration (temporary) (9999 if no follower 1) (front to front)
    double RelativeSpeedLeader2;    //At the end of the duration (temporary) (999 if no leader 2)
    double DistanceHeadwayLeader2;  //At the end of the duration (temporary) (9999 if no leader 2) (front to front)
    int LeadersChangingLanes; //Number of Leaders Who Changed Lanes During Duration
    
    double AverageSurroundingDensity; //At the end of the duration (temporary)
    double AverageSurroundingDensityLaneL; //At the end of the duration (temporary) (If no left lane: 9999.99)
    double AverageSurroundingDensityLaneR; //At the end of the duration (temporary) (If no right lane: 9999.99)
    double DistancetoEntrance; //At the end of the duration (temporary) (If no entrance: 999.99)
    
    double RelativeSpeedLeftL1;// With the Leader at the Target Lane to The Left (999 if No Leader Available)
    double DistanceHeadwayLeftL1;// 9999 if not available
    double RelativeSpeedRightL1;// With the Leader at the Target Lane to The Right (999 if No Leader Available)
    double DistanceHeadwayRightL1;// 9999 if not available
    double RelativeSpeedLeftF1;// With the Follower at the Target Lane to The Left (999 if No Leader Available)
    double DistanceHeadwayLeftF1;// 9999 if not available
    double RelativeSpeedRightF1;// With the Follower at the Target Lane to The Right (999 if No Leader Available)
    double DistanceHeadwayRightF1;// 9999 if not available
    //End of Added by Samer Hamdar, March 13, 2007
    
    int FirstEpisodeDummy; // 1 if First Episode, 0 otherwise. Added by Samer Hamdar, March 23, 2007
    
    //Added by Samer Hamdar: 20 September 2008
    int LeftLaneDummy;
    int RightLaneDummy;
    int RampDummy;
    
    double DistanceToRamp;
    //End of Added by Samer Hamdar: 20 September 2008
    
    //Added by Samer Hamdar: 21 September 2008
    int N_Period;
    //End of Added by Samer Hamdar: 21 September 2008
};
//End of Added by Samer Hamdar: 21 September 2008

struct VehDurationData
{
    int VehIDDuration;                //For Time Averaging Recognition: Added by Samer Hamdar, April 12, 2007 (21 September 2008)
    int CorrespondingFrameID;       //Frame ID Corresponding to the Duration, Added by Samer Hamdar, March 15, 2007
    int DurationID;                    //Duration ID
    int DurationTime;                //Duration Time (Multiple of 0.1 seconds)
    
    //Added by Samer Hamdar, March 13, 2007
    int EpisodeType;                //1- Exit by Changing Lanes from Car-Following Episode
    //2- Exit by Entering Free-Flow Episode from Car-Following Episode
    //3- Exit by Changing Lanes from Free-Flow Episode
    //4- Exit by Entering Car-Following Episode from Free-Flow Episode
    //5- Exit by Leaving Area from Car-Following Episode
    //3- Exit by Leaving Area from Free-Flow Episode
    int VehicleType;                // 1: Motorcycle 2: Auto 3: Truck (temporary)
    double DriverSpeed;             //At the end of the duration (temporary)
    double RelativeSpeedLeader1;    //At the end of the duration (temporary) (999 if no leader 1)
    double DistanceHeadwayLeader1;  //At the end of the duration (temporary) (9999 if no leader 1) (front to front)
    double SpaceSeparationLeader1;
    double RelativeSpeedFollower1;  //At the end of the duration (temporary) (999 if no follower 1)
    double DistanceHeadwayFollower1;//At the end of the duration (temporary) (9999 if no follower 1) (front to front)
    double RelativeSpeedLeader2;    //At the end of the duration (temporary) (999 if no leader 2)
    double DistanceHeadwayLeader2;  //At the end of the duration (temporary) (9999 if no leader 2) (front to front)
    int LeadersChangingLanes; //Number of Leaders Who Changed Lanes During Duration
    
    double AverageSurroundingDensity; //At the end of the duration (temporary)
    double AverageSurroundingDensityLaneL; //At the end of the duration (temporary) (If no left lane: 9999.99)
    double AverageSurroundingDensityLaneR; //At the end of the duration (temporary) (If no right lane: 9999.99)
    double DistancetoEntrance; //At the end of the duration (temporary) (If no entrance: 999.99)
    
    double RelativeSpeedLeftL1;// With the Leader at the Target Lane to The Left (999 if No Leader Available)
    double DistanceHeadwayLeftL1;// 9999 if not available
    double RelativeSpeedRightL1;// With the Leader at the Target Lane to The Right (999 if No Leader Available)
    double DistanceHeadwayRightL1;// 9999 if not available
    double RelativeSpeedLeftF1;// With the Follower at the Target Lane to The Left (999 if No Leader Available)
    double DistanceHeadwayLeftF1;// 9999 if not available
    double RelativeSpeedRightF1;// With the Follower at the Target Lane to The Right (999 if No Leader Available)
    double DistanceHeadwayRightF1;// 9999 if not available
    //End of Added by Samer Hamdar, March 13, 2007
    
    int FirstEpisodeDummy; // 1 if First Episode, 0 otherwise. Added by Samer Hamdar, March 23, 2007
    
    //Added by Samer Hamdar: 20 September 2008
    int LeftLaneDummy;
    int RightLaneDummy;
    int RampDummy;
    
    double DistanceToRamp;
    //End of Added by Samer Hamdar: 20 September 2008
    
    //Added by Samer Hamdar: 21 September 2008
    int N_Period;
    int N_Period_Permanent;
    vector<VehDurationDataSUB> VehDurationsSUB;
    //End of Added by Samer Hamdar: 21 September 2008
};

struct LaneChangingData
{
    int LaneChangingTime;
    double LaneChangingHazard;
    double CompetingHazard;
    int Mode;
};

//End of Added by Samer Hamdar: 6 October 2008
//_____________________________________________________________________________________

struct VehFrameData
{
    int VehIDFrame;                    //For Time Averaging Recognition: Added by Samer Hamdar, April 12, 2007
    int FrameGlobalID;              //For Future Reference in the Sorted Data; Added by Samer Hamdar, March 15, 2007
    int FrameID;                    //time
    int PreVehID;                    //Veh ahead
    int FollowVehID;                //Veh behind
    
    //Added by Samer Hamdar, March 13, 2007
    int GlobalTime;              //Check NGSIM-Data Definition
    double LocalX;
    double LocalY;
    double GlobalX;
    double GlobalY;
    double VehicleLength;
    double VehicleWidth;
    int VehicleClass;
    double VehicleVelocity;
    double VehicleAcceleration;
    int Lane;
    
    double SpaceHeadway;
    double TimeHeadway;
    //End of Added by Samer Hamdar, March 13, 2007
    
    double SpaceGap;
    double RelativeVelocity;
    
    double WallstenAcceleration;//Same as At: these group of variables are just for error computation: not the final ones
    double WallstenVelocity;
    double WallstenX;
    double WallstenGap;
    double WallstenRelativeSpeed;
    double WallstenSpaceHeadway;
    
    int DummyNoLeader;//dummy variable; = 0 if the wallsten value is computed or Leader Exists; = 1 if the wallsten value is not computed or no Leader
    double DummyWallstenMeasure;//a dummy value = -0.1 to be output instead of the Wallsten Measure if not computed (no leader or otherwsise)
    
    double PermanentWallstenAcceleration_V;//These are the final values when "optimal" chromosome found - Velocity
    double PermanentWallstenVelocity_V;
    double PermanentWallstenX_V;
    double PermanentWallstenGap_V;
    double PermanentWallstenRelativeSpeed_V;
    double PermanentWallstenSpaceHeadway_V;
    
    double PermanentWallstenAcceleration_S;//These are the final values when "optimal" chromosome found - Gap
    double PermanentWallstenVelocity_S;
    double PermanentWallstenX_S;
    double PermanentWallstenGap_S;
    double PermanentWallstenRelativeSpeed_S;
    double PermanentWallstenSpaceHeadway_S;
    
    //Added by Samer Hamdar: 6 October 2008
    int Mode;
    int Mode_Previous;
    
    double LaneChange_Hazard_CF;
    double Mode_Hazard_CF;
    double LaneChange_Hazard_FF;
    double Mode_Hazard_FF;
    //End of Added by Samer Hamdar: 6 October 2008
    
    //Added by Samer Hamdar: 7 October 2008
    int VehIDDuration_Frame;                //For Time Averaging Recognition: Added by Samer Hamdar, April 12, 2007 (21 September 2008)
    int CorrespondingFrameID_Frame;       //Frame ID Corresponding to the Duration, Added by Samer Hamdar, March 15, 2007
    int DurationID_Frame;                    //Duration ID
    int DurationTime_Frame;                //Duration Time (Multiple of 0.1 seconds)
    
    int EpisodeType_Frame;                //1- Exit by Changing Lanes from Car-Following Episode
    //2- Exit by Entering Free-Flow Episode from Car-Following Episode
    //3- Exit by Changing Lanes from Free-Flow Episode
    //4- Exit by Entering Car-Following Episode from Free-Flow Episode
    //5- Exit by Leaving Area from Car-Following Episode
    //3- Exit by Leaving Area from Free-Flow Episode
    int VehicleType_Frame;                // 1: Motorcycle 2: Auto 3: Truck (temporary)
    double DriverSpeed_Frame;             //At the end of the duration (temporary)
    double RelativeSpeedLeader1_Frame;    //At the end of the duration (temporary) (999 if no leader 1)
    double DistanceHeadwayLeader1_Frame;  //At the end of the duration (temporary) (9999 if no leader 1) (front to front)
    double SpaceSeparationLeader1_Frame;
    double RelativeSpeedFollower1_Frame;  //At the end of the duration (temporary) (999 if no follower 1)
    double DistanceHeadwayFollower1_Frame;//At the end of the duration (temporary) (9999 if no follower 1) (front to front)
    double RelativeSpeedLeader2_Frame;    //At the end of the duration (temporary) (999 if no leader 2)
    double DistanceHeadwayLeader2_Frame;  //At the end of the duration (temporary) (9999 if no leader 2) (front to front)
    int LeadersChangingLanes_Frame; //Number of Leaders Who Changed Lanes During Duration
    
    double AverageSurroundingDensity_Frame; //At the end of the duration (temporary)
    double AverageSurroundingDensityLaneL_Frame; //At the end of the duration (temporary) (If no left lane: 9999.99)
    double AverageSurroundingDensityLaneR_Frame; //At the end of the duration (temporary) (If no right lane: 9999.99)
    double DistancetoEntrance_Frame; //At the end of the duration (temporary) (If no entrance: 999.99)
    
    double RelativeSpeedLeftL1_Frame;// With the Leader at the Target Lane to The Left (999 if No Leader Available)
    double DistanceHeadwayLeftL1_Frame;// 9999 if not available
    double RelativeSpeedRightL1_Frame;// With the Leader at the Target Lane to The Right (999 if No Leader Available)
    double DistanceHeadwayRightL1_Frame;// 9999 if not available
    double RelativeSpeedLeftF1_Frame;// With the Follower at the Target Lane to The Left (999 if No Leader Available)
    double DistanceHeadwayLeftF1_Frame;// 9999 if not available
    double RelativeSpeedRightF1_Frame;// With the Follower at the Target Lane to The Right (999 if No Leader Available)
    double DistanceHeadwayRightF1_Frame;// 9999 if not available
    
    int FirstEpisodeDummy_Frame; // 1 if First Episode, 0 otherwise. Added by Samer Hamdar, March 23, 2007
    
    int LeftLaneDummy_Frame;
    int RightLaneDummy_Frame;
    int RampDummy_Frame;
    
    double DistanceToRamp_Frame;
    //End of Added by Samer Hamdar: 7 October 2008
    
    //Added by Samer Hamdar: 13 October 2008
    int Flag1;//= 1 if there is missing variable
    int Flag2;//= 1 if corresponds to the right most lane close interacting with the on-ramp
    //End of Added by Samer Hamdar: 13 October 2008
    
    int NumberOfLaneChanges_D;
};


// Define Vehicle Parameter Struct and Vector and Iclude in VehData struct
struct VehParameterData
{
    double Gamma1;
    double Gamma2;
    double PSI;
    double Wm;
    double Wc;
    double Tmax;
    double Alpha;
    double Beta;
    double Tcorr;
    
    double Ao;
    double So;
    double Vo;
    double Amax;
    
    double Wp;
    double DeltaW;
    double Sigma;
    
    int RT; //Reaction Time: Multiple of 0.1 (1 --> 0.1 s; 20 --> 2 s ...etc)
    
    double RelativeError_Velocity;
    double AbsoluteError_Velocity;
    double MixedError_Velocity;
    double RelativeError_Velocity_Validation;
    double AbsoluteError_Velocity_Validation;
    double MixedError_Velocity_Validation;
    
    double RelativeError_RelativeVelocity;
    double AbsoluteError_RelativeVelocity;
    double MixedError_RelativeVelocity;
    double RelativeError_RelativeVelocity_Validation;
    double AbsoluteError_RelativeVelocity_Validation;
    double MixedError_RelativeVelocity_Validation;
    
    double RelativeError_Gap;
    double AbsoluteError_Gap;
    double MixedError_Gap;
    double RelativeError_Gap_Validation;
    double AbsoluteError_Gap_Validation;
    double MixedError_Gap_Validation;
    
    int Accident;
};


struct VehData
{
    int VehID;
    int LeadersChanging;
    int NumFrames;
    int AccidentIndicator;// 1 if Accident, 0 Otherwise (if Accident: Velocity set to 0, Space Gap Set to 0, Acceleration Set to Zero)
    
    vector<VehFrameData> VehFrames;    //Vehicle frames
    int NumDurations;
    vector<VehDurationData> VehDurations;    //Vehicle duration
    
    //For Genetic Algorithm
    
    vector <VehParameterData> VehParameter;
    
    //Parents
    vector <VehParameterData> VehParameter_Mixed_V;//For Optimzing with Respect to Velocity Using Mixed Error
    vector <VehParameterData> VehParameter_Absolute_V;
    vector <VehParameterData> VehParameter_Relative_V;
    
    vector <VehParameterData> VehParameter_Mixed_S;
    vector <VehParameterData> VehParameter_Absolute_S;
    vector <VehParameterData> VehParameter_Relative_S;
    
    //Children
    vector <VehParameterData> VehParameterChildren_Mixed_V;
    vector <VehParameterData> VehParameterChildren_Absolute_V;
    vector <VehParameterData> VehParameterChildren_Relative_V;
    
    vector <VehParameterData> VehParameterChildren_Mixed_S;
    vector <VehParameterData> VehParameterChildren_Absolute_S;
    vector <VehParameterData> VehParameterChildren_Relative_S;
    
    
    //For Sensitivity Analysis (At this Stage: only around space gap); optimal solution with respect to:
    
    //Mixed Error
    vector<VehParameterData> VehParameterSensitivity_Gamma1_Mixed;
    vector<VehParameterData> VehParameterSensitivity_Gamma2_Mixed;
    vector<VehParameterData> VehParameterSensitivity_PSI_Mixed;
    vector<VehParameterData> VehParameterSensitivity_Wm_Mixed;
    vector<VehParameterData> VehParameterSensitivity_Wc_Mixed;
    vector<VehParameterData> VehParameterSensitivity_Tmax_Mixed;
    vector<VehParameterData> VehParameterSensitivity_Alpha_Mixed;
    vector<VehParameterData> VehParameterSensitivity_Beta_Mixed;
    vector<VehParameterData> VehParameterSensitivity_Tcorr_Mixed;
    vector<VehParameterData> VehParameterSensitivity_RT_Mixed;
    
    //Absolute Error
    
    vector<VehParameterData> VehParameterSensitivity_Gamma1_Absolute;
    vector<VehParameterData> VehParameterSensitivity_Gamma2_Absolute;
    vector<VehParameterData> VehParameterSensitivity_PSI_Absolute;
    vector<VehParameterData> VehParameterSensitivity_Wm_Absolute;
    vector<VehParameterData> VehParameterSensitivity_Wc_Absolute;
    vector<VehParameterData> VehParameterSensitivity_Tmax_Absolute;
    vector<VehParameterData> VehParameterSensitivity_Alpha_Absolute;
    vector<VehParameterData> VehParameterSensitivity_Beta_Absolute;
    vector<VehParameterData> VehParameterSensitivity_Tcorr_Absolute;
    vector<VehParameterData> VehParameterSensitivity_RT_Absolute;
    
    //Relative Error
    
    vector<VehParameterData> VehParameterSensitivity_Gamma1_Relative;
    vector<VehParameterData> VehParameterSensitivity_Gamma2_Relative;
    vector<VehParameterData> VehParameterSensitivity_PSI_Relative;
    vector<VehParameterData> VehParameterSensitivity_Wm_Relative;
    vector<VehParameterData> VehParameterSensitivity_Wc_Relative;
    vector<VehParameterData> VehParameterSensitivity_Tmax_Relative;
    vector<VehParameterData> VehParameterSensitivity_Alpha_Relative;
    vector<VehParameterData> VehParameterSensitivity_Beta_Relative;
    vector<VehParameterData> VehParameterSensitivity_Tcorr_Relative;
    vector<VehParameterData> VehParameterSensitivity_RT_Relative;
    
    //For Storing Optimal Solution in Terms of:
    
    //Velocity
    VehParameterData veh_parameter_data_best_absolute_V;
    VehParameterData veh_parameter_data_best_relative_V;
    VehParameterData veh_parameter_data_best_mixed_V;
    
    //Space Gap
    VehParameterData veh_parameter_data_best_absolute_S;
    VehParameterData veh_parameter_data_best_relative_S;
    VehParameterData veh_parameter_data_best_mixed_S;
    
    //Testing Purposes
    VehParameterData veh_parameter_data_tentative;
    
    //Added by Samer Hamdar: 6 October 2008
    vector<LaneChangingData> LaneChanging;
    int NumberOfLaneChanges;
    //End of Added by Samer Hamdar: 6 October 2008
    
    //Added by Samer Hamdar: 14 October 2008
    double GammaHeterogeneity_CF_LC;
    double GammaHeterogeneity_FF_LC;
    double GammaHeterogeneity_CF_CM;
    double GammaHeterogeneity_FF_CM;
    //End of Added by Samer Hamdar: 14 October 2008
};

vector<VehData> Vehicles;


//*****************************Data Structure Necessary for Sorting; Added by Samer Hamdar, March 19, 2007*********

vector<VehFrameData> FrameLocationInitial; // Initial Vector Needed so the frames are arranged based in their fame ID before being pushed to another set of vectors

struct FrameTimeData  //struct containing a vector of frames having the same time but arranged based on location
{
    vector <VehFrameData> FrameLocation; //Vector arranging the frames based on their X-Location at a Given Time
    double NumFramesLocation; //Number of Sorted Frames (in terms of x) in each time frame
};


vector<FrameTimeData> FrameTime; //the structs of "location frames" are sorted into a vector based on their time


//*****************************End of Data Structure Necessary for Sorting; Added by Samer Hamdar, March 19, 2007***
int main()
{
    double L = 550.0; //Length of the Study Area: Necessary for Interpolating Density Values: April 4, 2007
    int NLanes = 7; //Number of Lanes: Added by Samer Hamdar, April 4, 2007
    
    int FrameGlobalIDtmp;//Added by Samer Hamdar, March 15, 2007
    int NumberOfVehicles; //Added by Samer Hamdar, March 18, 2007
    double NumberOfFrames;//Added by Samer Hamdar, March 19, 2007
    
    int Parents = 10;
    int NumberOfChildren;
    
    int LeadersChangingtmp = 0;
    
    //___________Added by Samer Hamdar: 6 October 2008
    
    double PI = 3.141592653589793238462643383279502884197169399375;
    //double RoadLength = 550;//in meters
    double RampLength = 215;//in meters
    double DeltaT = 0.1;
    
    //________Duration Parameters
    
    //int Duration_Disaggregation = 50;//Data are recorded every "1 seconds" in duration
    
    //int DurationIDtmp = 0;
    //int DurationTimetmp = 0;
    
    //int EpisodeTypetmp;                    //1- Exit by Changing Lanes from Car-Following Episode
    //2- Exit by Entering Free-Flow Episode from Car-Following Episode
    //3- Exit by Changing Lanes from Free-Flow Episode
    //4- Exit by Entering Car-Following Episode from Free-Flow Episode
    //5- Exit by Leaving Area from Car-Following Episode
    //3- Exit by Leaving Area from Free-Flow Episode
    // (?): First Car is Assumed to Be Car-Following Even if Headway Not Known
    
    
    //int VehicleTypetmp;                    // 1: Motorcycle 2: Auto 3: Truck (temporary)
    //double DriverSpeedtmp;             //At the end of the duration (temporary)
    //double RelativeSpeedLeader1tmp;    //At the end of the duration (temporary) (999 if no leader 1)
    //double DistanceHeadwayLeader1tmp;  //At the end of the duration (temporary) (9999 if no leader 1) (front to front)
    //double SpaceSeparationLeader1tmp;
    //double RelativeSpeedFollower1tmp;  //At the end of the duration (temporary) (999 if no follower 1)
    //double DistanceHeadwayFollower1tmp;//At the end of the duration (temporary) (9999 if no follower 1) (front to front)
    //double RelativeSpeedLeader2tmp;    //At the end of the duration (temporary) (999 if no leader 2)
    //double DistanceHeadwayLeader2tmp;  //At the end of the duration (temporary) (9999 if no leader 2) (front to front)
    int LeadersChangingLanestmp; //Number of Leaders Who Changed Lanes During Duration
    
    double AverageSurroundingDensitytmp; //At the end of the duration (temporary)
    double AverageSurroundingDensityLaneLtmp; //At the end of the duration (temporary) (If no left lane: 9999.99)
    double AverageSurroundingDensityLaneRtmp; //At the end of the duration (temporary) (If no right lane: 9999.99)
    //double DistancetoEntrancetmp; //At the end of the duration (temporary) (If no entrance: 999.99)
    
    //double RelativeSpeedLeftL1tmp;// With the Leader at the Target Lane to The Left (999 if No Leader Available)
    //double DistanceHeadwayLeftL1tmp;// 9999 if not available
    //double RelativeSpeedRightL1tmp;// With the Leader at the Target Lane to The Right (999 if No Leader Available)
    //double DistanceHeadwayRightL1tmp;// 9999 if not available
    //double RelativeSpeedLeftF1tmp;// With the Follower at the Target Lane to The Left (999 if No Leader Available)
    //double DistanceHeadwayLeftF1tmp;// 9999 if not available
    //double RelativeSpeedRightF1tmp;// With the Follower at the Target Lane to The Right (999 if No Leader Available)
    //double DistanceHeadwayRightF1tmp;// 9999 if not available
    //End of Added by Samer Hamdar, March 13, 2007
    
    //double AverageSurroundingDensitytmpMajor; //For Averaging at the Last Time Steps: Added by Samer Hamdar, April 12, 2007 (21 September 2008)
    //double AverageSurroundingDensityLaneLtmpMajor; //For Averaging at the Last Time Steps: Added by Samer Hamdar, April 12, 2007 (21 September 2008)
    //double AverageSurroundingDensityLaneRtmpMajor; //For Averaging at the Last Time Steps: Added by Samer Hamdar, April 12, 2007 (21 September 2008)
    
    //____________Hazard Parameters: FF_CM -> Type 1, FF_LC -> Type 3, CF_CM -> Type 4, CF_LC -> Type 6
    
    //CF: Exit by Lane Change
    double Hazard_ParameterA_CF_LC = 0.0; //Alpha of the Duration Model: not needed in Limdep Implementation (Lambda)
    double Hazard_ParameterB_CF_LC = 1.5396; //Teta of the Duration Model (or P in Limdep Notation)
    
    double R_Heterogeneity_CF_LC = 1.0; //R Parameter for the Gamma Heterogeneity
    double K_Heterogeneity_CF_LC = 1.0; //K Parameter for the Gamma Heterogeneity
    
    //CF: Exit by Changing Modes
    double Hazard_ParameterA_CF_CM = 0.0; //Alpha of the Duration Model
    double Hazard_ParameterB_CF_CM = 1.5396; //Teta of the Duration Model
    
    double R_Heterogeneity_CF_CM = 1.0; //R Parameter for the Gamma Heterogeneity
    double K_Heterogeneity_CF_CM = 1.0; //K Parameter for the Gamma Heterogeneity
    
    //FF: Exit by Lane Change
    double Hazard_ParameterA_FF_LC = 0.0; //Alpha of the Duration Model
    double Hazard_ParameterB_FF_LC = 1.5396; //Teta of the Duration Model
    
    double R_Heterogeneity_FF_LC = 1.0; //R Parameter for the Gamma Heterogeneity
    double K_Heterogeneity_FF_LC = 1.0; //K Parameter for the Gamma Heterogeneity
    
    //FF: Exit by Changing Modes
    double Hazard_ParameterA_FF_CM = 0.0; //Alpha of the Duration Model
    double Hazard_ParameterB_FF_CM = 1.5396; //Teta of the Duration Model
    
    double R_Heterogeneity_FF_CM = 1.0; //R Parameter for the Gamma Heterogeneity
    double K_Heterogeneity_FF_CM = 1.0; //K Parameter for the Gamma Heterogeneity
    
    //Exogenous Covariates Parameters
    
    //___Car-Following Episodes
    
    //Exit by Lane Change
    double BetaOne_CF_LC = 0.0;
    
    double BetaLCL_CF_LC = -0.20842;
    
    double BetaV_CF_LC = 0.064285;
    
    double BetaDXL1_CF_LC = 0.027426;
    double BetaDVL1_CF_LC = -0.12671;
    
    double BetaDXL2_CF_LC = -0.00431;
    double BetaDVL2_CF_LC = -0.02568;
    
    double BetaDXF1_CF_LC = 0.0;
    double BetaDVF1_CF_LC = 0.043283;
    
    double BetaDXL1R_CF_LC = 0.0;
    double BetaDVL1R_CF_LC = -0.05779;
    double BetaDXF1R_CF_LC = 0.0;
    double BetaDVF1R_CF_LC = -0.05961;
    
    double BetaDXL1L_CF_LC = 0.0;
    double BetaDVL1L_CF_LC = -0.01916;
    double BetaDXF1L_CF_LC = -0.01189;
    double BetaDVF1L_CF_LC = 0.0;
    
    double BetaKL_CF_LC = 0.0;
    double BetaKR_CF_LC = 0.0;
    
    double BetaK_CF_LC = 0.07382;
    
    double BetaLeftLane_CF_LC = 0.0;
    double BetaRightLane_CF_LC = 0.0;
    double BetaRamp_CF_LC = 0.0;
    double BetaDistanceToRamp_CF_LC = 0.001227;
    //double BetaVehType_CF_LC = 0.0;
    
    //Exit by Changing Modes
    double BetaOne_CF_CM = 0.0;
    
    double BetaLCL_CF_CM = 0.274667;
    
    double BetaV_CF_CM = 0.064285;
    
    double BetaDXL1_CF_CM = -0.01163;
    double BetaDVL1_CF_CM = 0.0;
    
    double BetaDXL2_CF_CM = -0.00431;
    double BetaDVL2_CF_CM = -0.07738;
    
    double BetaDXF1_CF_CM = 0.0;
    double BetaDVF1_CF_CM = 0.0;
    
    double BetaDXL1R_CF_CM = 0.0;
    double BetaDVL1R_CF_CM = -0.05779;
    double BetaDXF1R_CF_CM = 0.0;
    double BetaDVF1R_CF_CM = 0.0;
    
    double BetaDXL1L_CF_CM = 0.0;
    double BetaDVL1L_CF_CM = 0.0;
    double BetaDXF1L_CF_CM = 0.013607;
    double BetaDVF1L_CF_CM = 0.0;
    
    double BetaKL_CF_CM = 0.0;
    double BetaKR_CF_CM = 0.0;
    
    double BetaK_CF_CM = 0.093976;
    
    double BetaLeftLane_CF_CM = 0.0;
    double BetaRightLane_CF_CM = 0.0;
    double BetaRamp_CF_CM = 0.0;
    double BetaDistanceToRamp_CF_CM = -0.00686;
    //double BetaVehType_CF_CM = 0.0;
    
    //________Free_Flow Episodes
    
    //Exit by Lane Change
    double BetaOne_FF_LC = 0.0;
    
    double BetaLCL_FF_LC = 0.274667;
    
    double BetaV_FF_LC = 0.064285;
    
    double BetaDXL1_FF_LC = 0.027426;
    double BetaDVL1_FF_LC = 0.0;
    
    double BetaDXL2_FF_LC = 0.00431;
    double BetaDVL2_FF_LC = 0.374553;
    
    double BetaDXF1_FF_LC = 0.0;
    double BetaDVF1_FF_LC = 0.0;
    
    double BetaDXL1R_FF_LC = 0.0;
    double BetaDVL1R_FF_LC = -0.05779;
    double BetaDXF1R_FF_LC = 0.0;
    double BetaDVF1R_FF_LC = 0.0;
    
    double BetaDXL1L_FF_LC = 0.0;
    double BetaDVL1L_FF_LC = 0.0;
    double BetaDXF1L_FF_LC = 0.013607;
    double BetaDVF1L_FF_LC = 0.0;
    
    double BetaKR_FF_LC = 0.0;
    double BetaKL_FF_LC = 0.0;
    
    double BetaK_FF_LC = 0.0;
    
    double BetaLeftLane_FF_LC = 0.0;
    double BetaRightLane_FF_LC = 0.0;
    double BetaRamp_FF_LC = 0.0;
    double BetaDistanceToRamp_FF_LC = -0.00686;
    //double BetaVehType_FF_LC = 0.0;
    
    //Exit by Changing Modes
    double BetaOne_FF_CM = 0.0;
    
    double BetaLCL_FF_CM = 0.274667;
    
    double BetaV_FF_CM = 0.064285;
    
    double BetaDXL1_FF_CM = 0.00523;
    double BetaDVL1_FF_CM = 0.0;
    
    double BetaDXL2_FF_CM = 0.001048;
    double BetaDVL2_FF_CM = -0.09938;
    
    double BetaDXF1_FF_CM = 0.0;
    double BetaDVF1_FF_CM = 0.0;
    
    double BetaDXL1R_FF_CM = 0.0;
    double BetaDVL1R_FF_CM = 0.05779;
    double BetaDXF1R_FF_CM = 0.0;
    double BetaDVF1R_FF_CM = 0.0;
    
    double BetaDXL1L_FF_CM = 0.0;
    double BetaDVL1L_FF_CM = 0.0;
    double BetaDXF1L_FF_CM = 0.013607;
    double BetaDVF1L_FF_CM = 0.0;
    
    double BetaKR_FF_CM = 0.0;
    double BetaKL_FF_CM = 0.0;
    
    double BetaK_FF_CM = 0.050802;
    
    double BetaLeftLane_FF_CM = 0.0;
    double BetaRightLane_FF_CM = 0.0;
    double BetaRamp_FF_CM = 0.0;
    double BetaDistanceToRamp_FF_CM = -0.00312;
    //double BetaVehType_FF_CM = 0.0;
    //End of Added by Samer Hamdar: 6 October 2008
    
    //Added by Samer Hamdar: 13 October 2008
    int x, y, sw, dummy, track; //General Purpose Variables for Data Retrieval
    //(t already defined: Number of FrameTime Data)
    //s: for defining the slots of the sorted data that need to be investigated (range of the vector)
    //dummy: recognize the presence of leaders/followers
    //track: variable to track n leaders ahead
    
    int ww;//Needed to be defined for the while loop; tracking of densities
    //int s;
    int x_c, y_c;
    
    double DistanceDensityAhead; //Distance within which the density needs to be computed ahead of the vehicle
    double DistanceDensityBehind;//Distance with which the density needs to be computed behind the vehicle
    
    DistanceDensityAhead = 250.0;
    DistanceDensityBehind = 250.0;
    //End of Added by Samer Hamdar: 13 October 2008
    
    //Added by Samer Hamdar: 13 October 2008
    //For Computing the Hazard with Heterogeneity
    double Hazard_tmp;
    double Survival_tmp;
    double Hazard2_tmp;
    double Survival2_tmp;
    double Lambda_tmp;
    double Teta_tmp;
    //double Gamma_tmp;//Test
    //double PSI_tmp;//Test
    //double V_tmp = ( (rand()%1000) / 1000.0 );//Test
    //double W_tmp;
    //End of Added by Samer Hamdar: 13 October 2008
    
    int vehIDtmp; //1
    int FrameIDtmp; //2
    int NumFramestmp; //3
    //Addded by Samer Hamdar, March 13, 2007
    int GlobalTimetmp; //4
    double LocalXtmp; //5
    double LocalYtmp; //6
    double GlobalXtmp; //7
    double GlobalYtmp; //8
    double VehicleLengthtmp; //9
    double VehicleWidthtmp; //10
    int VehicleClasstmp; //11
    double VehicleVelocitytmp; //12
    double VehicleAccelerationtmp; //13
    int Lanetmp; //14
    //End of Added by Samer Hamdar, March 13, 2007
    int PreVehIDtmp; //15
    int FollowVehIDtmp; //16
    //Added by Samer Hamdar, March 13, 2007
    double SpaceHeadwaytmp; //17
    double TimeHeadwaytmp; //18
    //End of Added by Samer Hamdar, March 13, 2007
    
    //double SpaceGaptmp;
    //double RelativeVelocitytmp;
    
    //Dummy Struct for the Sorting Logic
    VehParameterData veh_parameter_data_previous;
    
    //Initializing Variables for Wallsten Model
    double Tau = 0.0;
    double Seff;
    double X;
    
    double Z;
    double Zprime;
    double Zdoubleprime;
    double Zstar;
    double fn;
    //double Azero;
    
    //double G;
    //double Gprime;
    //double Gdoubleprime;
    double Uptprime;
    double Uptdoubleprime;
    
    double F;//Same as Uprime
    double Fprime;//Same as Udoubleprime
    
    //double Aone;
    //double Atwo;
    double Astar;//Same as Athree
    
    double Var;
    
    double Yt;
    double Yt_previous;
    double Leader_Previous;
    
    //Added by Samer Hamdar: 8 October 2008
    int Lane_Previous;
    double Mode_Percentage;
    //End of Added by Samer Hamdar: 8 October 2008
    
    //Added by Samer Hamdar: 8 October 2008
    double At_freeflow;
    //End of Added by Samer Hamdar: 8 October 2008
    
    //Added by Samer Hamdar: 15 October 2008
    //int FirstEpisodeDummytmp;
    //End of Added by Samer Hamdar: 15 October 2008
    
    //double At = 0.0;
    double At_Update = 0.0;
    //double Vt = 0.0;
    double Vt_Update = 0.0;
    //double Xt;
    double Xt_Update = 0.0;
    //double DeltaV;
    double DeltaV_Update = 0.0;
    //double DeltaX;
    double DeltaX_Update = 0.0;
    double S_Update = 0.0;
    //double DeltaS_Update;
    
    int RT_Update;
    int RT_Counter;
    
    double ErrorDummy1_V;
    double ErrorDummy2_V;
    double ErrorDummy3_V;
    double ErrorDummy4_V;
    double ErrorDummy5_V;
    double ErrorDummy1_V_Validation;
    double ErrorDummy2_V_Validation;
    double ErrorDummy3_V_Validation;
    double ErrorDummy4_V_Validation;
    double ErrorDummy5_V_Validation;
    
    double ErrorDummy1_DV;
    double ErrorDummy2_DV;
    double ErrorDummy3_DV;
    double ErrorDummy4_DV;
    double ErrorDummy5_DV;
    double ErrorDummy1_DV_Validation;
    double ErrorDummy2_DV_Validation;
    double ErrorDummy3_DV_Validation;
    double ErrorDummy4_DV_Validation;
    double ErrorDummy5_DV_Validation;
    
    double ErrorDummy1_S;
    double ErrorDummy2_S;
    double ErrorDummy3_S;
    double ErrorDummy4_S;
    double ErrorDummy5_S;
    double ErrorDummy1_S_Validation;
    double ErrorDummy2_S_Validation;
    double ErrorDummy3_S_Validation;
    double ErrorDummy4_S_Validation;
    double ErrorDummy5_S_Validation;
    
    //double AbsoluteErrorPrevious_V;//for sorting and finding the best chromosomes in terms of different errors
    //double RelativeErrorPrevious_V;
    //double MixedErrorPrevious_V;
    
    //double AbsoluteErrorPrevious_S;//for sorting and finding the best chromosomes in terms of different errors
    //double RelativeErrorPrevious_S;
    //double MixedErrorPrevious_S;
    
    //Initialization of Variable to Read Chromosome (Parameter) Data
    
    double Gamma1tmp;
    double Gamma2tmp;
    double PSItmp;
    double Wmtmp;
    double Wctmp;
    double Tmaxtmp;
    double Alphatmp;
    double Betatmp;
    double Tcorrtmp;
    
    int RTtmp;
    
    double Aotmp = 0.0;
    double Sotmp = 0.0;
    double Votmp = 0.0;
    
    
    double Wptmp = 0.0;
    //double DeltaWtmp;
    
    //Initialization of Variable to Read Chromosome Parameter for Sensitivty Analysis
    
    double Gamma1_D;
    double Gamma2_D;
    double PSI_D;
    double Wm_D;
    double Wc_D;
    double Tmax_D;
    double Alpha_D;
    double Beta_D;
    double Tcorr_D;
    
    //int RT_D;
    
    double Ao_D;
    double So_D;
    double Vo_D;
    double Amax_D;
    
    double Wp_D;
    double DeltaW_D;
    double Sigma_D;
    
    //Random Numbers for Generating Chromosomes and For The Wiener Process (7 parameters to calibrate)
    //double Random1 = 0.0;
    double Random1_1 = 0.0;
    double Random1_2 = 0.0;
    double Random2 = 0.0;
    double Random3 = 0.0;
    double Random4 = 0.0;
    double Random5 = 0.0;
    double Random6 = 0.0;
    double Random7 = 0.0;
    int Random8 = 0;
    double Random9 = 0.0;
    
    double Random_Wiener;
    
    int Random_CrossOver;
    int Random_Mutation;
    
    //double Random_Test;
    
    //Error Terms to Detect Convergence and Stopping Condition of the GA Algorithm
    
    double Error_Test;
    double Error_Test_Previous;
    int Error_Counter;
    double Error_Test_Threshold = 0.15;
    int Error_Counter_Threshold = 20;
    
    //Added by Samer Hamdar: 14 October 2008
    //int VehID_Calibrationtmp;// To Read from Error File
    //double ErrorStmp;
    //double ErrorVtmp;
    //double Sigmatmp;// = 0.5 * (1 - Gamma)
    double Amaxtmp = 0.0;
    
    int NumberOfVehicles_Calibration;
    int NumberOfVehicles_Relative;
    int NumberOfFrames_Relative;
    
    NumberOfVehicles_Calibration = 0;
    NumberOfVehicles_Relative = 0;
    NumberOfFrames_Relative = 0;
    //End of Added by Samer Hamdar: 14 October 2008
    
    
    //************************* Reading Data ***********************************
    
    FrameGlobalIDtmp = 0; //Added by Samer Hamdar, March 15, 2007
    NumberOfVehicles = 0; //Added by Samer Hamdar, March 18, 2007
    NumberOfFrames = 0; //Added by Samer Hamdar, March 19, 2007
    
    FILE *st,*out1_w, *out2_w, *out3_w, *out4_w;
    st = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/trajectories.txt", "r");
    out1_w = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/CalibratedTrajectories.txt", "w");
    out2_w = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/ErrorFile.txt", "w");
    out3_w = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/SensitivityFile.txt", "w");
    out4_w = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/LaneChangeHazardFile.txt", "w");
    
    //Added by Samer Hamdar: 14 October 2008
    FILE *out, *out_R, *out1, *out2, *out3, *out4, *out5, *out6;//Modified by Samer Hamdar: 19 September 2008
    FILE *out7_1, *out7_2, *out7_3, *out7_4, *out7_5, *out7_6;
    FILE *out8_1, *out8_2, *out8_3, *out8_4, *out8_5, *out8_6;
    FILE *out8_1R, *out8_2R, *out8_3R, *out8_4R, *out8_5R, *out8_6R;
    FILE *out9_1, *out9_2, *out9_3, *out9_4, *out9_5, *out9_6;
    FILE *out10_1, *out10_2, *out10_3, *out10_4, *out10_5, *out10_6;
    FILE *out_test_cf1, *out_test_cf2,*out_test_ff3, *out_test_ff4;
    
    out_test_cf1 =  fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_test_cf1.txt", "w");
    out_test_cf2 =  fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_test_cf2.txt", "w");
    out_test_ff3 =  fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_test_ff3.txt", "w");
    out_test_ff4 =  fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_test_ff4.txt", "w");
    
    out = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration.txt", "w");
    out_R = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/durationR.txt", "w");
    
    out1 = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_1.txt", "w");//Added by Samer Hamdar: 20 September 2008
    out2 = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_2.txt", "w");//Added by Samer Hamdar: 20 September 2008
    out3 = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_3.txt", "w");//Added by Samer Hamdar: 20 September 2008
    out4 = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_4.txt", "w");//Added by Samer Hamdar: 20 September 2008
    out5 = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_5.txt", "w");//Added by Samer Hamdar: 20 September 2008
    out6 = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_6.txt", "w");//Added by Samer Hamdar: 20 September 2008
    
    out7_1 = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_Ramp_1.txt", "w");//Added by Samer Hamdar: 20 September 2008
    out7_2 = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_Ramp_2.txt", "w");//Added by Samer Hamdar: 20 September 2008
    out7_3 = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_Ramp_3.txt", "w");//Added by Samer Hamdar: 20 September 2008
    out7_4 = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_Ramp_4.txt", "w");//Added by Samer Hamdar: 20 September 2008
    out7_5 = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_Ramp_5.txt", "w");//Added by Samer Hamdar: 20 September 2008
    out7_6 = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_Ramp_6.txt", "w");//Added by Samer Hamdar: 20 September 2008
    
    out8_1 = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_RightLane_1.txt", "w");//Added by Samer Hamdar: 20 September 2008
    out8_2 = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_RightLane_2.txt", "w");//Added by Samer Hamdar: 20 September 2008
    out8_3 = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_RightLane_3.txt", "w");//Added by Samer Hamdar: 20 September 2008
    out8_4 = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_RightLane_4.txt", "w");//Added by Samer Hamdar: 20 September 2008
    out8_5 = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_RightLane_5.txt", "w");//Added by Samer Hamdar: 20 September 2008
    out8_6 = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_RightLane_6.txt", "w");//Added by Samer Hamdar: 20 September 2008
    
    out8_1R = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_RightLane_1R.txt", "w");//Added by Samer Hamdar: 20 September 2008
    out8_2R = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_RightLane_2R.txt", "w");//Added by Samer Hamdar: 20 September 2008
    out8_3R = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_RightLane_3R.txt", "w");//Added by Samer Hamdar: 20 September 2008
    out8_4R = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_RightLane_4R.txt", "w");//Added by Samer Hamdar: 20 September 2008
    out8_5R = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_RightLane_5R.txt", "w");//Added by Samer Hamdar: 20 September 2008
    out8_6R = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_RightLane_6R.txt", "w");//Added by Samer Hamdar: 20 September 2008
    
    out9_1 = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_LeftLane_1.txt", "w");//Added by Samer Hamdar: 20 September 2008
    out9_2 = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_LeftLane_2.txt", "w");//Added by Samer Hamdar: 20 September 2008
    out9_3 = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_LeftLane_3.txt", "w");//Added by Samer Hamdar: 20 September 2008
    out9_4 = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_LeftLane_4.txt", "w");//Added by Samer Hamdar: 20 September 2008
    out9_5 = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_LeftLane_5.txt", "w");//Added by Samer Hamdar: 20 September 2008
    out9_6 = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_LeftLane_6.txt", "w");//Added by Samer Hamdar: 20 September 2008
    
    out10_1 = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_MiddleLanes_1.txt", "w");//Added by Samer Hamdar: 20 September 2008
    out10_2 = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_MiddleLanes_2.txt", "w");//Added by Samer Hamdar: 20 September 2008
    out10_3 = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_MiddleLanes_3.txt", "w");//Added by Samer Hamdar: 20 September 2008
    out10_4 = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_MiddleLanes_4.txt", "w");//Added by Samer Hamdar: 20 September 2008
    out10_5 = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_MiddleLanes_5.txt", "w");//Added by Samer Hamdar: 20 September 2008
    out10_6 = fopen("/Volumes/GoogleDrive/My Drive/Papers/ISTTT 2019/Calibration Data/duration_MiddleLanes_6.txt", "w");//Added by Samer Hamdar: 20 September 2008
    //End of Added by Samer Hamdar: 14 October 2008
    
    while(!feof(st))
    {
        //veh data
        VehData veh_data;
        
        //Added by Samer Hamdar: 14 October 2008
        veh_data.GammaHeterogeneity_CF_CM = GammaGenerate ( ( 1.0 / K_Heterogeneity_CF_CM ), R_Heterogeneity_CF_CM );
        veh_data.GammaHeterogeneity_CF_LC = GammaGenerate ( ( 1.0 / K_Heterogeneity_CF_LC ), R_Heterogeneity_CF_LC );
        veh_data.GammaHeterogeneity_FF_CM = GammaGenerate ( ( 1.0 / K_Heterogeneity_FF_CM ), R_Heterogeneity_FF_CM );
        veh_data.GammaHeterogeneity_FF_LC = GammaGenerate ( ( 1.0 / K_Heterogeneity_FF_LC ), R_Heterogeneity_FF_LC );
        
        veh_data.NumberOfLaneChanges = 0;
        //End of Added by Samer Hamdar: 14 October 2008
        
        veh_data.AccidentIndicator = 0;
        
        Gamma1tmp = 0.2;//Random 1
        Gamma2tmp = 0.2;//Random 1
        PSItmp = 2.0;//Random 1
        Wmtmp = 2.0;//Random 2
        Wctmp = 100000.0;//Random 3
        Tmaxtmp = 4.0;//Random 4
        Alphatmp = 0.08;//Random 5
        Betatmp = 5.0;//Random 6
        Tcorrtmp = 20.0;//Random 7
        RTtmp = 1;//Random 8
        
        Aotmp = 1.0;
        Sotmp = 3.0;
        Votmp = 108.8;
        Amaxtmp = 4.0;
        Wptmp = 1.0;
        
        //Initialized Parents for Optimzing with Respect to S: Mixed Error
        
        //The Tentative Best Chromosome (Testing Reasons)
        VehParameterData veh_parameter_data_tentative;
        
        veh_parameter_data_tentative.Gamma1 = 0.2;
        veh_parameter_data_tentative.Gamma2 = 0.2;
        veh_parameter_data_tentative.PSI = 2.0;
        veh_parameter_data_tentative.Wm = 2;
        veh_parameter_data_tentative.Wc = 100000;
        veh_parameter_data_tentative.Tmax = 4;
        veh_parameter_data_tentative.Alpha = 0.08;
        veh_parameter_data_tentative.Beta = 5;
        veh_parameter_data_tentative.Tcorr = 20;
        veh_parameter_data_tentative.RT = 1;
        
        veh_parameter_data_tentative.Ao = 1;
        veh_parameter_data_tentative.So = 3;
        veh_parameter_data_tentative.Vo = 108;
        veh_parameter_data_tentative.Amax = 1.5;
        veh_parameter_data_tentative.Wp = 1;
        
        veh_parameter_data_tentative.DeltaW = 1 - veh_parameter_data_tentative.Wm;
        veh_parameter_data_tentative.Sigma = ( ( 0.5 ) * ( 1 - veh_parameter_data_tentative.Gamma1 ) );
        
        veh_parameter_data_tentative.Accident = 1;
        
        veh_data.VehParameter_Mixed_S.push_back(veh_parameter_data_tentative);
        
        for (int i = 0; i < ( Parents - 1 ) ; i++)
        {
            VehParameterData veh_parameter_data;
            
            Random1_1 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
            Random1_2 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
            Random2 = ( ( rand() % 100 ) / 10  );// 0 --> 10; 1, 2, 3, ....
            Random3 = 50000 + ( ( (rand() % 10 ) / 10.0 ) * 100000 );// 50 000 --> 150 000 ; 50 000, 60 000, 70 000
            Random4 = 1 + ( ( (rand() % 10 ) / 10.0 ) * 10 ) ; // 1 --> 11; 1, 2, 3
            Random5 = ( (rand() % 50 ) / 10.0 ) / 10.0;// 0 --> 5; 0.01, 0.02, 0.03 ....
            Random6 = 1 + ( ( (rand() % 10 ) / 10.0 ) * 10 ) ; // 1 --> 11; 1, 2, 3
            Random7 = 10 + ( ( ( ( rand() % 20 )/ 10.0 ) ) * 10 );
            Random8 = ( ( rand() % 300 )/ 10.0 ); // 0 --> 30: 1, 2 ...
            Random9 = ( ( rand() % 100 ) / 10  );// 0 --> 10; 1, 2, 3, ....
            
            veh_parameter_data.Gamma1 = Random1_1;
            veh_parameter_data.Gamma2 = Random1_2;
            veh_parameter_data.PSI = Random9;
            veh_parameter_data.Wm = Random2;
            veh_parameter_data.Wc = Random3;
            veh_parameter_data.Tmax = Random4;
            veh_parameter_data.Alpha = Random5;
            veh_parameter_data.Beta = Random6;
            veh_parameter_data.Tcorr = Random7;
            veh_parameter_data.RT = Random8;
            
            ////Testing
            //veh_parameter_data.Gamma = 0.1;
            //veh_parameter_data.Wm = 9.0;
            //veh_parameter_data.Wc = 130000.0;
            //veh_parameter_data.Tmax = 7.0;
            //veh_parameter_data.Alpha = 0.03;
            //veh_parameter_data.Beta = 10.0;
            //veh_parameter_data.Tcorr = 14.0;
            //veh_parameter_data.RT = 0;
            
            veh_parameter_data.Ao = Aotmp;
            veh_parameter_data.So = Sotmp;
            veh_parameter_data.Vo = Votmp;
            veh_parameter_data.Amax = Amaxtmp;
            veh_parameter_data.Wp = Wptmp;
            
            veh_parameter_data.DeltaW = 1 - veh_parameter_data.Wm;
            veh_parameter_data.Sigma = ( ( 0.5 ) * ( 1 - veh_parameter_data.Gamma1 ) );
            
            veh_parameter_data.Accident = 0;
            
            veh_data.VehParameter_Mixed_S.push_back(veh_parameter_data);
        }
        
        
        //Initialized Parents for Optimzing with Respect to S: Absolute Error
        
        //The Tentative Best Chromosome (Testing Reasons)
        //        VehParameterData veh_parameter_data_tentative;
        
        veh_parameter_data_tentative.Gamma1 = 0.2;
        veh_parameter_data_tentative.Gamma2 = 0.2;
        veh_parameter_data_tentative.PSI = 2.0;
        veh_parameter_data_tentative.Wm = 2;
        veh_parameter_data_tentative.Wc = 100000;
        veh_parameter_data_tentative.Tmax = 4;
        veh_parameter_data_tentative.Alpha = 0.08;
        veh_parameter_data_tentative.Beta = 5;
        veh_parameter_data_tentative.Tcorr = 20;
        veh_parameter_data_tentative.RT = 1;
        
        veh_parameter_data_tentative.Ao = 1;
        veh_parameter_data_tentative.So = 3;
        veh_parameter_data_tentative.Vo = 108;
        veh_parameter_data_tentative.Amax = 1.5;
        veh_parameter_data_tentative.Wp = 1;
        
        veh_parameter_data_tentative.DeltaW = 1 - veh_parameter_data_tentative.Wm;
        veh_parameter_data_tentative.Sigma = ( ( 0.5 ) * ( 1 - veh_parameter_data_tentative.Gamma1 ) );
        
        veh_parameter_data_tentative.Accident = 1;
        
        veh_data.VehParameter_Absolute_S.push_back(veh_parameter_data_tentative);
        
        for (int i = 0; i < ( Parents - 1 ) ; i++)
        {
            VehParameterData veh_parameter_data;
            
            Random1_1 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
            Random1_2 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
            Random2 = ( ( rand() % 100 ) / 10  );// 0 --> 10; 1, 2, 3, ....
            Random3 = 50000 + ( ( (rand() % 10 ) / 10.0 ) * 100000 );// 50 000 --> 150 000 ; 50 000, 60 000, 70 000
            Random4 = 1 + ( ( (rand() % 10 ) / 10.0 ) * 10 ) ; // 1 --> 11; 1, 2, 3
            Random5 = ( (rand() % 50 ) / 10.0 ) / 10.0;// 0 --> 5; 0.01, 0.02, 0.03 ....
            Random6 = 1 + ( ( (rand() % 10 ) / 10.0 ) * 10 ) ; // 1 --> 11; 1, 2, 3
            Random7 = 10 + ( ( ( ( rand() % 20 )/ 10.0 ) ) * 10 );
            Random8 = ( ( rand() % 300 )/ 10.0 ); // 0 --> 30: 1, 2 ...
            
            veh_parameter_data.Gamma1 = Random1_1;
            veh_parameter_data.Gamma2 = Random1_2;
            veh_parameter_data.PSI = Random9;
            veh_parameter_data.Wm = Random2;
            veh_parameter_data.Wc = Random3;
            veh_parameter_data.Tmax = Random4;
            veh_parameter_data.Alpha = Random5;
            veh_parameter_data.Beta = Random6;
            veh_parameter_data.Tcorr = Random7;
            veh_parameter_data.RT = Random8;
            
            ////Testing
            //veh_parameter_data.Gamma = 0.1;
            //veh_parameter_data.Wm = 9.0;
            //veh_parameter_data.Wc = 130000.0;
            //veh_parameter_data.Tmax = 7.0;
            //veh_parameter_data.Alpha = 0.03;
            //veh_parameter_data.Beta = 10.0;
            //veh_parameter_data.Tcorr = 14.0;
            //veh_parameter_data.RT = 0;
            
            veh_parameter_data.Ao = Aotmp;
            veh_parameter_data.So = Sotmp;
            veh_parameter_data.Vo = Votmp;
            veh_parameter_data.Amax = Amaxtmp;
            veh_parameter_data.Wp = Wptmp;
            
            veh_parameter_data.DeltaW = 1 - veh_parameter_data.Wm;
            veh_parameter_data.Sigma = ( ( 0.5 ) * ( 1 - veh_parameter_data.Gamma1 ) );
            
            veh_parameter_data.Accident = 0;
            
            veh_data.VehParameter_Absolute_S.push_back(veh_parameter_data);
        }
        
        
        //Initialized Parents for Optimzing with Respect to S: Relative Error
        
        //The Tentative Best Chromosome (Testing Reasons)
        //        VehParameterData veh_parameter_data_tentative;
        
        veh_parameter_data_tentative.Gamma1 = 0.2;
        veh_parameter_data_tentative.Gamma2 = 0.2;
        veh_parameter_data_tentative.PSI= 2.0;
        veh_parameter_data_tentative.Wm = 2;
        veh_parameter_data_tentative.Wc = 100000;
        veh_parameter_data_tentative.Tmax = 4;
        veh_parameter_data_tentative.Alpha = 0.08;
        veh_parameter_data_tentative.Beta = 5;
        veh_parameter_data_tentative.Tcorr = 20;
        veh_parameter_data_tentative.RT = 1;
        
        veh_parameter_data_tentative.Ao = 1;
        veh_parameter_data_tentative.So = 3;
        veh_parameter_data_tentative.Vo = 108;
        veh_parameter_data_tentative.Amax = 1.5;
        veh_parameter_data_tentative.Wp = 1;
        
        veh_parameter_data_tentative.DeltaW = 1 - veh_parameter_data_tentative.Wm;
        veh_parameter_data_tentative.Sigma = ( ( 0.5 ) * ( 1 - veh_parameter_data_tentative.Gamma1 ) );
        
        veh_parameter_data_tentative.Accident = 1;
        
        veh_data.VehParameter_Relative_S.push_back(veh_parameter_data_tentative);
        for (int i = 0; i < ( Parents - 1 ) ; i++)
        {
            VehParameterData veh_parameter_data;
            
            Random1_1 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
            Random1_2 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
            Random2 = ( ( rand() % 100 ) / 10  );// 0 --> 10; 1, 2, 3, ....
            Random3 = 50000 + ( ( (rand() % 10 ) / 10.0 ) * 100000 );// 50 000 --> 150 000 ; 50 000, 60 000, 70 000
            Random4 = 1 + ( ( (rand() % 10 ) / 10.0 ) * 10 ) ; // 1 --> 11; 1, 2, 3
            Random5 = ( (rand() % 50 ) / 10.0 ) / 10.0;// 0 --> 5; 0.01, 0.02, 0.03 ....
            Random6 = 1 + ( ( (rand() % 10 ) / 10.0 ) * 10 ) ; // 1 --> 11; 1, 2, 3
            Random7 = 10 + ( ( ( ( rand() % 20 )/ 10.0 ) ) * 10 );
            Random8 = ( ( rand() % 300 )/ 10.0 ); // 0 --> 30: 1, 2 ...
            
            veh_parameter_data.Gamma1 = Random1_1;
            veh_parameter_data.Gamma2 = Random1_2;
            veh_parameter_data.PSI = Random9;
            veh_parameter_data.Wm = Random2;
            veh_parameter_data.Wc = Random3;
            veh_parameter_data.Tmax = Random4;
            veh_parameter_data.Alpha = Random5;
            veh_parameter_data.Beta = Random6;
            veh_parameter_data.Tcorr = Random7;
            veh_parameter_data.RT = Random8;
            
            ////Testing
            //veh_parameter_data.Gamma = 0.1;
            //veh_parameter_data.Wm = 9.0;
            //veh_parameter_data.Wc = 130000.0;
            //veh_parameter_data.Tmax = 7.0;
            //veh_parameter_data.Alpha = 0.03;
            //veh_parameter_data.Beta = 10.0;
            //veh_parameter_data.Tcorr = 14.0;
            //veh_parameter_data.RT = 0;
            
            veh_parameter_data.Ao = Aotmp;
            veh_parameter_data.So = Sotmp;
            veh_parameter_data.Vo = Votmp;
            veh_parameter_data.Amax = Amaxtmp;
            veh_parameter_data.Wp = Wptmp;
            
            veh_parameter_data.DeltaW = 1 - veh_parameter_data.Wm;
            veh_parameter_data.Sigma = ( ( 0.5 ) * ( 1 - veh_parameter_data.Gamma1 ) );
            
            veh_parameter_data.Accident = 0;
            
            veh_data.VehParameter_Relative_S.push_back(veh_parameter_data);
        }
        
        // read first row of a veh
        vehIDtmp = g_Readnum(st); //1
        FrameIDtmp = g_Readnum(st); //2
        NumFramestmp = g_Readnum(st); //3
        //Modified by Samer Hamdar, March 13, 2007
        GlobalTimetmp = g_Readnum(st);      //4
        LocalXtmp = g_Readdouble(st); //5
        LocalYtmp = g_Readdouble(st); //6
        GlobalXtmp = g_Readdouble(st); //7
        GlobalYtmp = g_Readdouble(st); //8
        VehicleLengthtmp = g_Readdouble(st); //9
        VehicleWidthtmp = g_Readdouble(st); //10
        VehicleClasstmp = g_Readnum(st); //11
        VehicleVelocitytmp = g_Readdouble(st); //12
        VehicleAccelerationtmp = g_Readdouble(st); //13
        Lanetmp = g_Readnum(st); //14
        //End of Modified by Samer Hamdar, March 13, 2007
        PreVehIDtmp  = g_Readnum(st); //15
        FollowVehIDtmp = g_Readnum(st);  //16
        //Modified by Samer Hamdar, March 13, 2007
        SpaceHeadwaytmp = g_Readdouble(st); //17
        TimeHeadwaytmp = g_Readdouble(st); //18
        //End of Modified by Samer Hamdar, March 13, 2007
        
        //        SpaceGaptmp = g_Readdouble(st);
        //        RelativeVelocitytmp = g_Readdouble(st);
        
        NumberOfFrames = 0;
        LeadersChangingtmp = 0; // Added by Samer Hamdar, March 23, 2007
        
        //veh
        veh_data.VehID = vehIDtmp; //1
        veh_data.NumFrames = NumFramestmp; //3
        
        //veh frame
        VehFrameData veh_frame_data;
        veh_frame_data.VehIDFrame = vehIDtmp; //For Time Averaging Recognition: Added by Samer Hamdar, April 12, 2007
        veh_frame_data.FrameID = FrameIDtmp; //2
        //Added by Samer Hamdar, March 13, 2007
        veh_frame_data.GlobalTime = GlobalTimetmp; //4
        veh_frame_data.LocalX = LocalXtmp * 0.30479999798832; //5 (Conversion from Feet to Meters)
        veh_frame_data.LocalY = LocalYtmp * 0.30479999798832; //6 (Conversion from Feet to Meters)
        veh_frame_data.GlobalX = GlobalXtmp * 0.30479999798832; //7 (Conversion from Feet to Meters)
        veh_frame_data.GlobalY = GlobalYtmp * 0.30479999798832; //8 (Conversion from Feet to Meters)
        veh_frame_data.VehicleLength = VehicleLengthtmp * 0.30479999798832; //9 (Conversion from Feet to Meters)
        veh_frame_data.VehicleWidth = VehicleWidthtmp * 0.30479999798832; //10 (Conversion from Feet to Meters)
        veh_frame_data.VehicleClass = VehicleClasstmp; //11
        veh_frame_data.VehicleVelocity = VehicleVelocitytmp * 0.30479999798832; //12 (Conversion from Ft/s m/s)
        veh_frame_data.VehicleAcceleration = VehicleAccelerationtmp * 0.30479999798832; //13 (Conversion from Ft/s2 to m/s2)
        veh_frame_data.Lane = Lanetmp; //14
        //End of Added by Samer Hamdar, March 13, 2007
        veh_frame_data.PreVehID = PreVehIDtmp; //15
        veh_frame_data.FollowVehID = FollowVehIDtmp; //16
        //Added by Samer Hamdar, March 13, 2007
        veh_frame_data.SpaceHeadway = SpaceHeadwaytmp * 0.30479999798832; //17
        veh_frame_data.TimeHeadway = TimeHeadwaytmp; //18
        //End of Added by Samer Hamdar, March 13, 2007
        
        //        veh_frame_data.SpaceGap = SpaceGaptmp * 0.30479999798832;
        //        veh_frame_data.RelativeVelocity = RelativeVelocitytmp * 0.30479999798832;
        
        //Added by Samer Hamdar: 8 October 2008
        veh_frame_data.VehicleType_Frame = veh_frame_data.VehicleClass;
        veh_frame_data.DriverSpeed_Frame = veh_frame_data.VehicleVelocity;
        veh_frame_data.DistanceHeadwayLeader1_Frame = veh_frame_data.SpaceHeadway;
        veh_frame_data.DistancetoEntrance_Frame = veh_frame_data.LocalY;
        veh_frame_data.DistanceToRamp_Frame = fabs(RampLength - veh_frame_data.LocalY);
        veh_frame_data.FirstEpisodeDummy_Frame = 1;
        veh_frame_data.LeadersChangingLanes_Frame = 0;
        
        if (veh_frame_data.Lane == 7)
            veh_frame_data.RampDummy_Frame = 1;
        else
            veh_frame_data.RampDummy_Frame = 0;
        
        if (veh_frame_data.Lane == 1)
            veh_frame_data.LeftLaneDummy_Frame = 1;
        else
            veh_frame_data.LeftLaneDummy_Frame = 0;
        
        if (veh_frame_data.Lane == 6)
            veh_frame_data.RightLaneDummy_Frame = 1;
        else
            veh_frame_data.RightLaneDummy_Frame = 0;
        
        if (veh_frame_data.Lane == 9)
            veh_frame_data.RightLaneDummy_Frame = 1;
        else
            veh_frame_data.RightLaneDummy_Frame = 0;
        
        veh_frame_data.Mode = 1;
        veh_frame_data.Mode_Previous = -1;
        veh_frame_data.LaneChange_Hazard_CF = 0.0;
        veh_frame_data.Mode_Hazard_CF = 0.0;
        veh_frame_data.LaneChange_Hazard_FF = 0.0;
        veh_frame_data.Mode_Hazard_FF = 0.0;
        //End Added by Samer Hamdar: 8 October 2008
        
        //add to vehFrames
        veh_data.VehFrames.push_back(veh_frame_data);
        
        // read other rows of a veh
        for (int i=1;i<veh_data.NumFrames;i++)
        {
            vehIDtmp = g_Readnum(st); //1
            FrameIDtmp = g_Readnum(st); //2
            NumFramestmp = g_Readnum(st); //3
            //Modified by Samer Hamdar, March 13, 2007
            GlobalTimetmp = g_Readnum(st);      //4
            LocalXtmp = g_Readdouble(st); //5
            LocalYtmp = g_Readdouble(st); //6
            GlobalXtmp = g_Readdouble(st); //7
            GlobalYtmp = g_Readdouble(st); //8
            VehicleLengthtmp = g_Readdouble(st); //9
            VehicleWidthtmp = g_Readdouble(st); //10
            VehicleClasstmp = g_Readnum(st); //11
            VehicleVelocitytmp = g_Readdouble(st); //12
            VehicleAccelerationtmp = g_Readdouble(st); //13
            Lanetmp = g_Readnum(st); //14
            //End of Modified by Samer Hamdar, March 13, 2007
            PreVehIDtmp  = g_Readnum(st); //15
            FollowVehIDtmp = g_Readnum(st);  //16
            //Modified by Samer Hamdar, March 13, 2007
            SpaceHeadwaytmp = g_Readdouble(st); //17
            TimeHeadwaytmp = g_Readdouble(st); //18
            //End of Modified by Samer Hamdar, March 13, 2007
            
            //            SpaceGaptmp = g_Readdouble(st);
            //            RelativeVelocitytmp = g_Readdouble(st);
            
            //veh frame
            VehFrameData veh_frame_data;
            veh_frame_data.VehIDFrame = vehIDtmp; //For Time Averaging Recognition: Added by Samer Hamdar, April 12, 2007
            veh_frame_data.FrameID = FrameIDtmp; //2
            //Added by Samer Hamdar, March 13, 2007
            veh_frame_data.GlobalTime = GlobalTimetmp; //4
            veh_frame_data.LocalX = LocalXtmp * 0.30479999798832; //5 (Conversion from Feet to Meters)
            veh_frame_data.LocalY = LocalYtmp * 0.30479999798832; //6 (Conversion from Feet to Meters)
            veh_frame_data.GlobalX = GlobalXtmp * 0.30479999798832; //7 (Conversion from Feet to Meters)
            veh_frame_data.GlobalY = GlobalYtmp * 0.30479999798832; //8 (Conversion from Feet to Meters)
            veh_frame_data.VehicleLength = VehicleLengthtmp * 0.30479999798832; //9 (Conversion from Feet to Meters)
            veh_frame_data.VehicleWidth = VehicleWidthtmp * 0.30479999798832; //10 (Conversion from Feet to Meters)
            veh_frame_data.VehicleClass = VehicleClasstmp; //11
            veh_frame_data.VehicleVelocity = VehicleVelocitytmp * 0.30479999798832; //12 (Conversion from Ft/s Km/h)
            veh_frame_data.VehicleAcceleration = VehicleAccelerationtmp * 0.30479999798832; //13 (Conversion from Ft/s2 to m/s2)
            veh_frame_data.Lane = Lanetmp; //14
            //End of Added by Samer Hamdar, March 13, 2007
            veh_frame_data.PreVehID = PreVehIDtmp; //15
            veh_frame_data.FollowVehID = FollowVehIDtmp; //16
            //Added by Samer Hamdar, March 13, 2007
            veh_frame_data.SpaceHeadway = SpaceHeadwaytmp * 0.30479999798832;; //17
            veh_frame_data.TimeHeadway = TimeHeadwaytmp; //18
            //End of Added by Samer Hamdar, March 13, 2007
            
            //            veh_frame_data.SpaceGap = SpaceGaptmp * 0.30479999798832;
            //            veh_frame_data.RelativeVelocity = RelativeVelocitytmp * 0.30479999798832;
            
            veh_frame_data.FrameGlobalID = FrameGlobalIDtmp;//Added by Samer Hamdar, March 15, 2007
            FrameGlobalIDtmp = FrameGlobalIDtmp + 1;  //Added by Samer Hamdar, March 15, 2007
            //            NumberOfFrames = NumberOfFrames + 1; //Added by Samer Hamdar, March 19, 2007
            
            //Added by Samer Hamdar: 8 October 2008
            veh_frame_data.VehicleType_Frame = veh_frame_data.VehicleClass;
            veh_frame_data.DriverSpeed_Frame = veh_frame_data.VehicleVelocity;
            veh_frame_data.DistanceHeadwayLeader1_Frame = veh_frame_data.SpaceHeadway;
            veh_frame_data.DistancetoEntrance_Frame = veh_frame_data.LocalY;
            veh_frame_data.DistanceToRamp_Frame = fabs(RampLength - veh_frame_data.LocalY);
            veh_frame_data.FirstEpisodeDummy_Frame = 1;
            veh_frame_data.LeadersChangingLanes_Frame = 0;
            
            if (veh_frame_data.Lane == 7)
                veh_frame_data.RampDummy_Frame = 1;
            else
                veh_frame_data.RampDummy_Frame = 0;
            
            if (veh_frame_data.Lane == 1)
                veh_frame_data.LeftLaneDummy_Frame = 1;
            else
                veh_frame_data.LeftLaneDummy_Frame = 0;
            
            if (veh_frame_data.Lane == 6)
                veh_frame_data.RightLaneDummy_Frame = 1;
            else
                veh_frame_data.RightLaneDummy_Frame = 0;
            
            if (veh_frame_data.Lane == 9)
                veh_frame_data.RightLaneDummy_Frame = 1;
            else
                veh_frame_data.RightLaneDummy_Frame = 0;
            
            veh_frame_data.Mode = 1;
            veh_frame_data.Mode_Previous = -1;
            veh_frame_data.LaneChange_Hazard_CF = 0.0;
            veh_frame_data.Mode_Hazard_CF = 0.0;
            veh_frame_data.LaneChange_Hazard_FF = 0.0;
            veh_frame_data.Mode_Hazard_FF = 0.0;
            //End Added by Samer Hamdar: 8 October 2008
            
            //add to vehFrames
            veh_data.VehFrames.push_back(veh_frame_data);
            
            //Re-Added by Samer Hamdar, March 23 (Added March 13), 2007: Save Last Record Instead of the First
            VehicleClasstmp = veh_data.VehFrames[i-1].VehicleClass;
            VehicleVelocitytmp = veh_data.VehFrames[i-1].VehicleVelocity;
            SpaceHeadwaytmp = veh_data.VehFrames[i-1].SpaceHeadway;
            //Re-Added by Samer Hamdar, March 23 (Added March 13), 2007: Save Last Record Instead of the First
            
            if ((veh_data.VehFrames[i-1].PreVehID != veh_data.VehFrames[i].PreVehID))//Added by Samer Hamdar, March 23, 2007
            {
                LeadersChangingtmp = LeadersChangingtmp + 1;//Added by Samer Hamdar, March 23, 2007
                
            }
            
        }
        NumberOfVehicles = NumberOfVehicles + 1;//Added by Samer Hamdar, March 18, 2007
        veh_data.LeadersChanging = LeadersChangingtmp;
        
        Vehicles.push_back(veh_data);
        
    }
    //*************************End of Reading Data **********************************************************
    
    
    //************************Specifying The Relative Measures (Velocities)**********************************
    
    //Added by Samer Hamdar: 8 October 2008
    int Match_V;
    int Match_F;
    int VehicleCounter;
    int FrameCounter;
    
    //for (int n = 0; n < 3 ; n++)//NumOfVehicles is over-estimated by 1
    for (int n = 0; n < NumberOfVehicles - 1; n++)//NumOfVehicles is over-estimated by 1
    {
        cout<<n<<endl;
        
        Match_V = 0;
        Match_F = 0;
        
        for (int f = 0; f < Vehicles[n].NumFrames ; f++)
        {
            
            if ( f != 0)
            {
                if ( Vehicles[n].VehFrames[f].PreVehID != Vehicles[n].VehFrames[f-1].PreVehID)
                {
                    Match_V = 0;
                    Match_F = 0;
                }
            }
            
            if ( Vehicles[n].VehFrames[f].PreVehID != 0)
            {
                VehicleCounter = Match_V;
                
                while ( Vehicles[n].VehFrames[f].PreVehID != Vehicles[VehicleCounter].VehID && VehicleCounter < NumberOfVehicles - 2)
                {
                    VehicleCounter = VehicleCounter + 1;
                }
                
                Match_V = VehicleCounter;
                
                FrameCounter = Match_F;
                
                //int sss = Vehicles[VehicleCounter].NumFrames;
                
                //CHECK THE CODE: I have changes "Vehicles[VehicleCounter].NumFrames - 1" check with 57
                while( Vehicles[VehicleCounter].VehFrames[FrameCounter].FrameID != Vehicles[n].VehFrames[f].FrameID  && FrameCounter < Vehicles[VehicleCounter].NumFrames - 1 )
                {
                    FrameCounter = FrameCounter + 1;
                }
                
                Vehicles[n].VehFrames[f].RelativeVelocity = ( Vehicles[n].VehFrames[f].VehicleVelocity - Vehicles[VehicleCounter].VehFrames[FrameCounter].VehicleVelocity);
                Vehicles[n].VehFrames[f].SpaceGap = ( Vehicles[VehicleCounter].VehFrames[FrameCounter].LocalY - Vehicles[VehicleCounter].VehFrames[FrameCounter].VehicleLength - Vehicles[n].VehFrames[f].LocalY );
                
                if ( Vehicles[n].VehFrames[f].SpaceGap <=0)
                    Vehicles[n].VehFrames[f].SpaceGap = 0.1;
                
                //Added by Samer Hamdar
                Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame = Vehicles[n].VehFrames[f].RelativeVelocity;
                Vehicles[n].VehFrames[f].SpaceSeparationLeader1_Frame = Vehicles[n].VehFrames[f].SpaceGap;
                //End of Added by Samer Hamdar
                
                Match_F = FrameCounter;
                
            }
            else
            {
                Vehicles[n].VehFrames[f].RelativeVelocity = -20;
                Vehicles[n].VehFrames[f].SpaceGap = -0.1;
                
                //Added by Samer Hamdar
                Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame = Vehicles[n].VehFrames[f].RelativeVelocity;
                Vehicles[n].VehFrames[f].SpaceSeparationLeader1_Frame = Vehicles[n].VehFrames[f].SpaceGap;
                //End of Added by Samer Hamdar
            }
        }
    }
    //End of Added by Samer Hamdar: 8 October 2008
    
    
    //***********************End of Specifying The Relative Measures (Velocity)*******************************
    
    //**********************Rearrange Vehicles in New Structure for Hazard Computation************************
    //6 October 2008
    
    //Added by Samer Hamdar: 23 September 2008
    //long GIGI = FrameLocationInitial.max_size();
    //Vehicles2.clear();
    //Vehicles1.clear();
    //End of Added by Samer Hamdar: 23 September 2008
    
    int l = 0; // Counters in space
    int t = 0; // Counters in time
    
    VehFrameData temptime; //Dummy Struct for Bubble Sorting in the time dimension-Added by Samer Hamdar, March 18-2007
    VehFrameData templocation; //Dummny Struct for Bubble Sorting in the Space Dimension-Added by Samer Hamdar, March 18-2007
    
    FrameLocationInitial.reserve(1300000);
    
    //Copy Vehicle Frames into another vector
    //    for (int n = 0; n < NumberOfVehicles - 1 ; n++)//NumOfVehicles is over-estimated by 2
    for (int n = 0; n < NumberOfVehicles - 2; n++)//NumOfVehicles is over-estimated by 2
    {
        //if (n == 1417)
        //{
        //    int tracker = 1;
        //}
        for (int f = 0; f < Vehicles[n].NumFrames ; f++)
        {
            //if (n == 1417 && f == 206)
            //{
            //    int tracker = 1;
            //}
            
            //Assign all previously read frames to a new set of vector with one time slot t = 0
            VehFrameData veh_frame_data3;
            veh_frame_data3 = Vehicles[n].VehFrames[f];
            
            //Modified by Samer Hamdar: 23 September 2008
            FrameLocationInitial.push_back(veh_frame_data3);
            
            //if (n < 1000)
            //    FrameLocationInitial.push_back(veh_frame_data3);
            //else
            //    FrameLocationInitial2.push_back(veh_frame_data3);
            //End of Modified by Samer Hamdar: 23 September 2008
            
            
            l = l+1;
        }
    }
    
    ////Added: Trial by Samer Hamdar, March 28, 2007: Special Sort Algorithm
    int vglobalcounter = 1;
    int fglobalcounter = Vehicles[vglobalcounter - 1].NumFrames;
    
    int v = vglobalcounter;//Variable to jump through vehicle stack of frames
    int f = fglobalcounter;
    int r; //counter for the start of the search
    // r counter for tracking frames from the beginning
    //int framecounter; // detecting the frame from which sliding will start
    //int dummymultiple = 2; //dummy variable allowing to insert frames on their sorted location
    //int y;
    
    //while (vglobalcounter < NumberOfVehicles - 1)
    while (vglobalcounter < NumberOfVehicles - 2)
    {
        //cout<<vglobalcounter<<endl;
        
        v = vglobalcounter;
        f = fglobalcounter;
        if    (FrameLocationInitial[f].FrameID < FrameLocationInitial[f-1].FrameID)//if time greater than the previous time - no need to sort
        {
            r = 0;
            while ( f < fglobalcounter + Vehicles[vglobalcounter].NumFrames)
            {
                while (r < f)//f
                {
                    //time less than the previous time: 2 cases
                    //case 1:
                    if (FrameLocationInitial[f].FrameID < FrameLocationInitial[r].FrameID)
                    {//time is less than the time corresonding to r
                        temptime = FrameLocationInitial[f];
                        for (int SlideCounter = 0; SlideCounter < (f - r); SlideCounter++)
                        {
                            FrameLocationInitial[f - SlideCounter] = FrameLocationInitial[f - SlideCounter - 1];
                        }
                        FrameLocationInitial[r] = temptime;
                        break;
                    }
                    else
                    {//time greater than the time corresponding to r: do nothing
                        
                    }
                    r++;
                    
                }
                f++;
            }
        }
        vglobalcounter = vglobalcounter + 1;
        fglobalcounter = fglobalcounter + Vehicles[vglobalcounter - 1].NumFrames;
    }
    //End of Added Trial by Samer Hamdar, March 28, 2007
    
    //Group the Sorted Vector of Frames Into Sub-Vectors Based on their Time
    int NumFramesLocationtmp = 0;
    FrameTimeData frame_time_data;
    NumberOfFrames = fglobalcounter;
    
    
    frame_time_data.FrameLocation.reserve(1190000);
    FrameTime.reserve(1190000);
    
    //    for (int i = 0; i < NumberOfFrames - 2; i++)
    for (int i = 0; i < NumberOfFrames - 2; i++)
    {
        VehFrameData veh_frame_data;
        NumFramesLocationtmp = NumFramesLocationtmp + 1;
        veh_frame_data = FrameLocationInitial[i];
        frame_time_data.FrameLocation.push_back(veh_frame_data);
        if (FrameLocationInitial[i].FrameID != FrameLocationInitial[i+1].FrameID)
        {
            frame_time_data.NumFramesLocation = NumFramesLocationtmp;
            FrameTime.push_back(frame_time_data);
            frame_time_data.FrameLocation.clear();
            FrameTimeData frame_time_data;
            NumFramesLocationtmp = 0;
            t = t+1;
        }
        //cout<<i<<endl;
        
        //        if ( i == NumberOfFrames - 3)//Necessary to Read the Last Item
        if ( i == NumberOfFrames - 3)//Necessary to Read the Last Item
        {
            veh_frame_data = FrameLocationInitial[i+1];
            if (FrameLocationInitial[i].FrameID != FrameLocationInitial[i+1].FrameID)
            {
                frame_time_data.FrameLocation.push_back(veh_frame_data);
                NumFramesLocationtmp = 1;
                frame_time_data.NumFramesLocation = NumFramesLocationtmp;
                FrameTime.push_back(frame_time_data);
                t = t+1;
            }
            else
            {
                frame_time_data.FrameLocation.push_back(veh_frame_data);
                NumFramesLocationtmp = NumFramesLocationtmp + 1;
                frame_time_data.NumFramesLocation = NumFramesLocationtmp;
            }
        }
    }
    
    //Arrange the frames in each time-subvector based on their location (FrameLocation)- Bubble Sort
    
    for (int n = 0; n < t; n++)
    {
        for (int i = 0; i < FrameTime[n].NumFramesLocation - 1; i++)
        {
            for (int j = i+1; j > 0; --j)
            {
                if (FrameTime[n].NumFramesLocation > 1 && FrameTime[n].FrameLocation[j].LocalY < FrameTime[n].FrameLocation[j-1].LocalY)
                {
                    //swap values
                    templocation = FrameTime[n].FrameLocation[j];
                    FrameTime[n].FrameLocation[j] = FrameTime[n].FrameLocation[j - 1];
                    FrameTime[n].FrameLocation[j - 1] = templocation;
                }
            }
        }
    }
    //6 October 2008
    //**********************End of Rearrange Vehicles in New Structure for Hazard Computation*************
    
    //************************Computing Wallsten Car-Following Measures and Error Terms/Perform Cross-Overs ************************
    
    //Test Only First Vehicle
    //    for (int n = 0; n < 3 ; n++)//NumOfVehicles is over-estimated by 2
    //for (int n = 0; n < NumberOfVehicles - 2; n++)//NumOfVehicles is over-estimated by 2: It should be NumberOfVehicles - 2
    int number_of_cars_to_be_analyzed = 200;
    for (int n = 0; n < number_of_cars_to_be_analyzed; n++)//NumOfVehicles is over-estimated by 2: It should be NumberOfVehicles - 2
    {
        
        //******Mixed Error GA Algorithm - Gap*****
        
        Error_Test = 99999.9;
        Error_Counter = 0;
        
        Vehicles[n].veh_parameter_data_best_mixed_S = Vehicles[n].VehParameter_Mixed_S[Parents - 1];//Initially: Assign the Tentative veh_parameter_data (Treiber's Values) to the first of the Children Chromosome
        
        //Here to Start for the While Loop for the Error
        
        while ( ( Error_Counter < Error_Counter_Threshold ) && ( (fabs (Error_Test)) > Error_Test_Threshold ) )
        {
            
            //cout<<n<<"        "<<Error_Counter<<endl;
            
            x_c = 0;
            y_c = 0;
            
            Vehicles[n].VehParameterChildren_Mixed_S.clear();
            
            Vehicles[n].VehParameterChildren_Mixed_S.push_back(Vehicles[n].veh_parameter_data_best_mixed_S);//Keep the Best Chromosome from the Earlier Iteration in the Children Pool
            
            //Create Children Chromosomes (Mating)
            
            NumberOfChildren = 1;
            
            for (int s = 0; s < Parents; s++)
            {
                for (int t = ( s + 1 ); t < Parents; t++)
                {
                    //Performing the CrossOver
                    Random_CrossOver =  ( 1 + ( ( rand() % 80 ) / 10 ) );
                    
                    if (Random_CrossOver == 1)
                    {
                        //First Chromosome
                        VehParameterData veh_parameter_data;
                        
                        veh_parameter_data.Gamma1 = Vehicles[n].VehParameter_Mixed_S[s].Gamma1;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Gamma1 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
                        
                        veh_parameter_data.Gamma2 = Vehicles[n].VehParameter_Mixed_S[s].Gamma2;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Gamma2 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
                        
                        veh_parameter_data.PSI = Vehicles[n].VehParameter_Mixed_S[t].PSI;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.PSI = ( ( rand() % 100 ) / 10.0  );// 0 --> 10; 0.1, 0.2, 0.3, ....
                        
                        veh_parameter_data.Wm = Vehicles[n].VehParameter_Mixed_S[t].Wm;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Wm = ( ( rand() % 100 ) / 10.0  );// 0 --> 10; 0.1, 0.2, 0.3, ....
                        
                        veh_parameter_data.Wc = Vehicles[n].VehParameter_Mixed_S[t].Wc;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Wc = 50000 + ( ( ( rand() % 100 ) / 10.0  ) * 10000 );// 50 000 --> 150 000 ; 51000, 52000, 53000
                        
                        veh_parameter_data.Tmax = Vehicles[n].VehParameter_Mixed_S[t].Tmax;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Tmax = 1 + ((rand() % 100 )/10.0 ) ; // 1 --> 11; 1.1, 1.2, 1.3
                        
                        veh_parameter_data.Alpha = Vehicles[n].VehParameter_Mixed_S[t].Alpha;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Alpha = ( (rand() % 50 ) / 10.0 ) / 10.0;// 0 --> 5; 0.01, 0.02, 0.03 ....
                        
                        veh_parameter_data.Beta = Vehicles[n].VehParameter_Mixed_S[t].Beta;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Beta = 1 + ((rand() % 100 )/10.0 ) ; // 1 --> 11; 1.1, 1.2, 1.3
                        
                        veh_parameter_data.Tcorr = Vehicles[n].VehParameter_Mixed_S[t].Tcorr;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Tcorr = 10 + ( ( ( ( rand() % 20 )/ 10.0 ) ) * 10 );
                        
                        veh_parameter_data.RT = Vehicles[n].VehParameter_Mixed_S[t].RT;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.RT = ( ( rand() % 300 )/ 10.0 ); // 0 --> 30: 1, 2 ...
                        
                        veh_parameter_data.Ao = Aotmp;
                        veh_parameter_data.So = Sotmp;
                        veh_parameter_data.Vo = Votmp;
                        veh_parameter_data.Amax = Amaxtmp;
                        veh_parameter_data.Wp = Wptmp;
                        
                        veh_parameter_data.DeltaW = 1 - veh_parameter_data.Wm;
                        veh_parameter_data.Sigma = ( ( 0.5 ) * ( 1 - veh_parameter_data.Gamma1 ) );
                        
                        Vehicles[n].VehParameterChildren_Mixed_S.push_back(veh_parameter_data);
                        NumberOfChildren = NumberOfChildren + 1;
                        
                        //Second Chromosome
                        //VehParameterData veh_parameter_data;
                        
                        veh_parameter_data.Gamma1 = Vehicles[n].VehParameter_Mixed_S[t].Gamma1;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Gamma1 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
                        
                        veh_parameter_data.Gamma2 = Vehicles[n].VehParameter_Mixed_S[t].Gamma2;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Gamma2 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
                        
                        veh_parameter_data.PSI = Vehicles[n].VehParameter_Mixed_S[s].PSI;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.PSI = ( ( rand() % 100 ) / 10.0  );// 0 --> 10; 0.1, 0.2, 0.3, ....
                        
                        veh_parameter_data.Wm = Vehicles[n].VehParameter_Mixed_S[s].Wm;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Wm = ( ( rand() % 100 ) / 10.0  );// 0 --> 10; 0.1, 0.2, 0.3, ....
                        
                        veh_parameter_data.Wc = Vehicles[n].VehParameter_Mixed_S[s].Wc;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Wc = 50000 + ( ( ( rand() % 100 ) / 10.0  ) * 10000 );// 50 000 --> 150 000 ; 51000, 52000, 53000
                        
                        veh_parameter_data.Tmax = Vehicles[n].VehParameter_Mixed_S[s].Tmax;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Tmax = 1 + ((rand() % 100 )/10.0 ) ; // 1 --> 11; 1.1, 1.2, 1.3
                        
                        veh_parameter_data.Alpha = Vehicles[n].VehParameter_Mixed_S[s].Alpha;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Alpha = ( (rand() % 50 ) / 10.0 ) / 10.0;// 0 --> 5; 0.01, 0.02, 0.03 ....
                        
                        veh_parameter_data.Beta = Vehicles[n].VehParameter_Mixed_S[s].Beta;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Beta = 1 + ((rand() % 100 )/10.0 ) ; // 1 --> 11; 1.1, 1.2, 1.3
                        
                        veh_parameter_data.Tcorr = Vehicles[n].VehParameter_Mixed_S[s].Tcorr;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Tcorr = 10 + ( ( ( ( rand() % 20 )/ 10.0 ) ) * 10 );
                        
                        veh_parameter_data.RT = Vehicles[n].VehParameter_Mixed_S[s].RT;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.RT = ( ( rand() % 300 )/ 10.0 ); // 0 --> 30: 1, 2 ...
                        
                        veh_parameter_data.Ao = Aotmp;
                        veh_parameter_data.So = Sotmp;
                        veh_parameter_data.Vo = Votmp;
                        veh_parameter_data.Amax = Amaxtmp;
                        veh_parameter_data.Wp = Wptmp;
                        
                        veh_parameter_data.DeltaW = 1 - veh_parameter_data.Wm;
                        veh_parameter_data.Sigma = ( ( 0.5 ) * ( 1 - veh_parameter_data.Gamma1 ) );
                        
                        Vehicles[n].VehParameterChildren_Mixed_S.push_back(veh_parameter_data);
                        NumberOfChildren = NumberOfChildren + 1;
                    }
                    
                    if (Random_CrossOver == 2)
                    {
                        //First Chromosome
                        VehParameterData veh_parameter_data;
                        
                        veh_parameter_data.Gamma1 = Vehicles[n].VehParameter_Mixed_S[s].Gamma1;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Gamma1 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
                        
                        veh_parameter_data.Gamma2 = Vehicles[n].VehParameter_Mixed_S[s].Gamma2;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Gamma2 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
                        
                        veh_parameter_data.PSI = Vehicles[n].VehParameter_Mixed_S[t].PSI;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.PSI = ( ( rand() % 100 ) / 10.0  );// 0 --> 10; 0.1, 0.2, 0.3, ....
                        
                        veh_parameter_data.Wm = Vehicles[n].VehParameter_Mixed_S[s].Wm;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Wm = ( ( rand() % 100 ) / 10.0  );// 0 --> 10; 0.1, 0.2, 0.3, ....
                        
                        veh_parameter_data.Wc = Vehicles[n].VehParameter_Mixed_S[t].Wc;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Wc = 50000 + ( ( ( rand() % 100 ) / 10.0  ) * 10000 );// 50 000 --> 150 000 ; 51000, 52000, 53000
                        
                        veh_parameter_data.Tmax = Vehicles[n].VehParameter_Mixed_S[t].Tmax;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Tmax = 1 + ((rand() % 100 )/10.0 ) ; // 1 --> 11; 1.1, 1.2, 1.3
                        
                        veh_parameter_data.Alpha = Vehicles[n].VehParameter_Mixed_S[t].Alpha;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Alpha = ( (rand() % 50 ) / 10.0 ) / 10.0;// 0 --> 5; 0.01, 0.02, 0.03 ....
                        
                        veh_parameter_data.Beta = Vehicles[n].VehParameter_Mixed_S[t].Beta;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Beta = 1 + ((rand() % 100 )/10.0 ) ; // 1 --> 11; 1.1, 1.2, 1.3
                        
                        veh_parameter_data.Tcorr = Vehicles[n].VehParameter_Mixed_S[t].Tcorr;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Tcorr = 10 + ( ( ( ( rand() % 20 )/ 10.0 ) ) * 10 );
                        
                        veh_parameter_data.RT = Vehicles[n].VehParameter_Mixed_S[t].RT;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.RT = ( ( rand() % 300 )/ 10.0 ); // 0 --> 30: 1, 2 ...
                        
                        veh_parameter_data.Ao = Aotmp;
                        veh_parameter_data.So = Sotmp;
                        veh_parameter_data.Vo = Votmp;
                        veh_parameter_data.Amax = Amaxtmp;
                        veh_parameter_data.Wp = Wptmp;
                        
                        veh_parameter_data.DeltaW = 1 - veh_parameter_data.Wm;
                        veh_parameter_data.Sigma = ( ( 0.5 ) * ( 1 - veh_parameter_data.Gamma1 ) );
                        
                        Vehicles[n].VehParameterChildren_Mixed_S.push_back(veh_parameter_data);
                        NumberOfChildren = NumberOfChildren + 1;
                        
                        //Second Chromosome
                        //VehParameterData veh_parameter_data;
                        
                        veh_parameter_data.Gamma1 = Vehicles[n].VehParameter_Mixed_S[t].Gamma1;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Gamma1 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
                        
                        veh_parameter_data.Gamma2 = Vehicles[n].VehParameter_Mixed_S[t].Gamma2;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Gamma2 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
                        
                        veh_parameter_data.PSI = Vehicles[n].VehParameter_Mixed_S[s].PSI;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.PSI = ( ( rand() % 100 ) / 10.0  );// 0 --> 10; 0.1, 0.2, 0.3, ....
                        
                        veh_parameter_data.Wm = Vehicles[n].VehParameter_Mixed_S[t].Wm;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Wm = ( ( rand() % 100 ) / 10.0  );// 0 --> 10; 0.1, 0.2, 0.3, ....
                        
                        veh_parameter_data.Wc = Vehicles[n].VehParameter_Mixed_S[s].Wc;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Wc = 50000 + ( ( ( rand() % 100 ) / 10.0  ) * 10000 );// 50 000 --> 150 000 ; 51000, 52000, 53000
                        
                        veh_parameter_data.Tmax = Vehicles[n].VehParameter_Mixed_S[s].Tmax;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Tmax = 1 + ((rand() % 100 )/10.0 ) ; // 1 --> 11; 1.1, 1.2, 1.3
                        
                        veh_parameter_data.Alpha = Vehicles[n].VehParameter_Mixed_S[s].Alpha;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Alpha = ( (rand() % 50 ) / 10.0 ) / 10.0;// 0 --> 5; 0.01, 0.02, 0.03 ....
                        
                        veh_parameter_data.Beta = Vehicles[n].VehParameter_Mixed_S[s].Beta;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Beta = 1 + ((rand() % 100 )/10.0 ) ; // 1 --> 11; 1.1, 1.2, 1.3
                        
                        veh_parameter_data.Tcorr = Vehicles[n].VehParameter_Mixed_S[s].Tcorr;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Tcorr = 10 + ( ( ( ( rand() % 20 )/ 10.0 ) ) * 10 );
                        
                        veh_parameter_data.RT = Vehicles[n].VehParameter_Mixed_S[s].RT;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.RT = ( ( rand() % 300 )/ 10.0 ); // 0 --> 30: 1, 2 ...
                        
                        veh_parameter_data.Ao = Aotmp;
                        veh_parameter_data.So = Sotmp;
                        veh_parameter_data.Vo = Votmp;
                        veh_parameter_data.Amax = Amaxtmp;
                        veh_parameter_data.Wp = Wptmp;
                        
                        veh_parameter_data.DeltaW = 1 - veh_parameter_data.Wm;
                        veh_parameter_data.Sigma = ( ( 0.5 ) * ( 1 - veh_parameter_data.Gamma1) );
                        
                        Vehicles[n].VehParameterChildren_Mixed_S.push_back(veh_parameter_data);
                        NumberOfChildren = NumberOfChildren + 1;
                    }
                    
                    if (Random_CrossOver == 3)
                    {
                        //First Chromosome
                        VehParameterData veh_parameter_data;
                        
                        veh_parameter_data.Gamma1 = Vehicles[n].VehParameter_Mixed_S[s].Gamma1;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Gamma1 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
                        
                        veh_parameter_data.Gamma2 = Vehicles[n].VehParameter_Mixed_S[s].Gamma2;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Gamma2 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
                        
                        veh_parameter_data.PSI = Vehicles[n].VehParameter_Mixed_S[t].PSI;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.PSI = ( ( rand() % 100 ) / 10.0  );// 0 --> 10; 0.1, 0.2, 0.3, ....
                        
                        veh_parameter_data.Wm = Vehicles[n].VehParameter_Mixed_S[s].Wm;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Wm = ( ( rand() % 100 ) / 10.0  );// 0 --> 10; 0.1, 0.2, 0.3, ....
                        
                        veh_parameter_data.Wc = Vehicles[n].VehParameter_Mixed_S[s].Wc;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Wc = 50000 + ( ( ( rand() % 100 ) / 10.0  ) * 10000 );// 50 000 --> 150 000 ; 51000, 52000, 53000
                        
                        veh_parameter_data.Tmax = Vehicles[n].VehParameter_Mixed_S[t].Tmax;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Tmax = 1 + ((rand() % 100 )/10.0 ) ; // 1 --> 11; 1.1, 1.2, 1.3
                        
                        veh_parameter_data.Alpha = Vehicles[n].VehParameter_Mixed_S[t].Alpha;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Alpha = ( (rand() % 50 ) / 10.0 ) / 10.0;// 0 --> 5; 0.01, 0.02, 0.03 ....
                        
                        veh_parameter_data.Beta = Vehicles[n].VehParameter_Mixed_S[t].Beta;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Beta = 1 + ((rand() % 100 )/10.0 ) ; // 1 --> 11; 1.1, 1.2, 1.3
                        
                        veh_parameter_data.Tcorr = Vehicles[n].VehParameter_Mixed_S[t].Tcorr;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Tcorr = 10 + ( ( ( ( rand() % 20 )/ 10.0 ) ) * 10 );
                        
                        veh_parameter_data.RT = Vehicles[n].VehParameter_Mixed_S[t].RT;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.RT = ( ( rand() % 300 )/ 10.0 ); // 0 --> 30: 1, 2 ...
                        
                        veh_parameter_data.Ao = Aotmp;
                        veh_parameter_data.So = Sotmp;
                        veh_parameter_data.Vo = Votmp;
                        veh_parameter_data.Amax = Amaxtmp;
                        veh_parameter_data.Wp = Wptmp;
                        
                        veh_parameter_data.DeltaW = 1 - veh_parameter_data.Wm;
                        veh_parameter_data.Sigma = ( ( 0.5 ) * ( 1 - veh_parameter_data.Gamma1 ) );
                        
                        Vehicles[n].VehParameterChildren_Mixed_S.push_back(veh_parameter_data);
                        NumberOfChildren = NumberOfChildren + 1;
                        
                        //Second Chromosome
                        //VehParameterData veh_parameter_data;
                        
                        veh_parameter_data.Gamma1 = Vehicles[n].VehParameter_Mixed_S[t].Gamma1;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Gamma1 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
                        
                        veh_parameter_data.Gamma2 = Vehicles[n].VehParameter_Mixed_S[t].Gamma2;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Gamma2 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
                        
                        veh_parameter_data.PSI = Vehicles[n].VehParameter_Mixed_S[s].PSI;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.PSI = ( ( rand() % 100 ) / 10.0  );// 0 --> 10; 0.1, 0.2, 0.3, ....
                        
                        veh_parameter_data.Wm = Vehicles[n].VehParameter_Mixed_S[t].Wm;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Wm = ( ( rand() % 100 ) / 10.0  );// 0 --> 10; 0.1, 0.2, 0.3, ....
                        
                        veh_parameter_data.Wc = Vehicles[n].VehParameter_Mixed_S[t].Wc;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Wc = 50000 + ( ( ( rand() % 100 ) / 10.0  ) * 10000 );// 50 000 --> 150 000 ; 51000, 52000, 53000
                        
                        veh_parameter_data.Tmax = Vehicles[n].VehParameter_Mixed_S[s].Tmax;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Tmax = 1 + ((rand() % 100 )/10.0 ) ; // 1 --> 11; 1.1, 1.2, 1.3
                        
                        veh_parameter_data.Alpha = Vehicles[n].VehParameter_Mixed_S[s].Alpha;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Alpha = ( (rand() % 50 ) / 10.0 ) / 10.0;// 0 --> 5; 0.01, 0.02, 0.03 ....
                        
                        veh_parameter_data.Beta = Vehicles[n].VehParameter_Mixed_S[s].Beta;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Beta = 1 + ((rand() % 100 )/10.0 ) ; // 1 --> 11; 1.1, 1.2, 1.3
                        
                        veh_parameter_data.Tcorr = Vehicles[n].VehParameter_Mixed_S[s].Tcorr;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Tcorr = 10 + ( ( ( ( rand() % 20 )/ 10.0 ) ) * 10 );
                        
                        veh_parameter_data.RT = Vehicles[n].VehParameter_Mixed_S[s].RT;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.RT = ( ( rand() % 300 )/ 10.0 ); // 0 --> 30: 1, 2 ...
                        
                        veh_parameter_data.Ao = Aotmp;
                        veh_parameter_data.So = Sotmp;
                        veh_parameter_data.Vo = Votmp;
                        veh_parameter_data.Amax = Amaxtmp;
                        veh_parameter_data.Wp = Wptmp;
                        
                        veh_parameter_data.DeltaW = 1 - veh_parameter_data.Wm;
                        veh_parameter_data.Sigma = ( ( 0.5 ) * ( 1 - veh_parameter_data.Gamma1 ) );
                        
                        Vehicles[n].VehParameterChildren_Mixed_S.push_back(veh_parameter_data);
                        NumberOfChildren = NumberOfChildren + 1;
                    }
                    
                    if (Random_CrossOver == 4)
                    {
                        //First Chromosome
                        VehParameterData veh_parameter_data;
                        
                        veh_parameter_data.Gamma1 = Vehicles[n].VehParameter_Mixed_S[s].Gamma1;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Gamma1 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
                        
                        veh_parameter_data.Gamma2 = Vehicles[n].VehParameter_Mixed_S[s].Gamma2;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Gamma2 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
                        
                        veh_parameter_data.PSI = Vehicles[n].VehParameter_Mixed_S[t].PSI;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.PSI = ( ( rand() % 100 ) / 10.0  );// 0 --> 10; 0.1, 0.2, 0.3, ....
                        
                        veh_parameter_data.Wm = Vehicles[n].VehParameter_Mixed_S[s].Wm;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Wm = ( ( rand() % 100 ) / 10.0  );// 0 --> 10; 0.1, 0.2, 0.3, ....
                        
                        veh_parameter_data.Wc = Vehicles[n].VehParameter_Mixed_S[s].Wc;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Wc = 50000 + ( ( ( rand() % 100 ) / 10.0  ) * 10000 );// 50 000 --> 150 000 ; 51000, 52000, 53000
                        
                        veh_parameter_data.Tmax = Vehicles[n].VehParameter_Mixed_S[s].Tmax;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Tmax = 1 + ((rand() % 100 )/10.0 ) ; // 1 --> 11; 1.1, 1.2, 1.3
                        
                        veh_parameter_data.Alpha = Vehicles[n].VehParameter_Mixed_S[t].Alpha;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Alpha = ( (rand() % 50 ) / 10.0 ) / 10.0;// 0 --> 5; 0.01, 0.02, 0.03 ....
                        
                        veh_parameter_data.Beta = Vehicles[n].VehParameter_Mixed_S[t].Beta;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Beta = 1 + ((rand() % 100 )/10.0 ) ; // 1 --> 11; 1.1, 1.2, 1.3
                        
                        veh_parameter_data.Tcorr = Vehicles[n].VehParameter_Mixed_S[t].Tcorr;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Tcorr = 10 + ( ( ( ( rand() % 20 )/ 10.0 ) ) * 10 );
                        
                        veh_parameter_data.RT = Vehicles[n].VehParameter_Mixed_S[t].RT;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.RT = ( ( rand() % 300 )/ 10.0 ); // 0 --> 30: 1, 2 ...
                        
                        veh_parameter_data.Ao = Aotmp;
                        veh_parameter_data.So = Sotmp;
                        veh_parameter_data.Vo = Votmp;
                        veh_parameter_data.Amax = Amaxtmp;
                        veh_parameter_data.Wp = Wptmp;
                        
                        veh_parameter_data.DeltaW = 1 - veh_parameter_data.Wm;
                        veh_parameter_data.Sigma = ( ( 0.5 ) * ( 1 - veh_parameter_data.Gamma1 ) );
                        
                        Vehicles[n].VehParameterChildren_Mixed_S.push_back(veh_parameter_data);
                        NumberOfChildren = NumberOfChildren + 1;
                        
                        //Second Chromosome
                        //VehParameterData veh_parameter_data;
                        
                        veh_parameter_data.Gamma1 = Vehicles[n].VehParameter_Mixed_S[t].Gamma1;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Gamma1= ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
                        
                        veh_parameter_data.Gamma2 = Vehicles[n].VehParameter_Mixed_S[t].Gamma2;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Gamma2 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
                        
                        veh_parameter_data.PSI = Vehicles[n].VehParameter_Mixed_S[s].PSI;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.PSI = ( ( rand() % 100 ) / 10.0  );// 0 --> 10; 0.1, 0.2, 0.3, ....
                        
                        veh_parameter_data.Wm = Vehicles[n].VehParameter_Mixed_S[t].Wm;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Wm = ( ( rand() % 100 ) / 10.0  );// 0 --> 10; 0.1, 0.2, 0.3, ....
                        
                        veh_parameter_data.Wc = Vehicles[n].VehParameter_Mixed_S[t].Wc;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Wc = 50000 + ( ( ( rand() % 100 ) / 10.0  ) * 10000 );// 50 000 --> 150 000 ; 51000, 52000, 53000
                        
                        veh_parameter_data.Tmax = Vehicles[n].VehParameter_Mixed_S[t].Tmax;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Tmax = 1 + ((rand() % 100 )/10.0 ) ; // 1 --> 11; 1.1, 1.2, 1.3
                        
                        veh_parameter_data.Alpha = Vehicles[n].VehParameter_Mixed_S[s].Alpha;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Alpha = ( (rand() % 50 ) / 10.0 ) / 10.0;// 0 --> 5; 0.01, 0.02, 0.03 ....
                        
                        veh_parameter_data.Beta = Vehicles[n].VehParameter_Mixed_S[s].Beta;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Beta = 1 + ((rand() % 100 )/10.0 ) ; // 1 --> 11; 1.1, 1.2, 1.3
                        
                        veh_parameter_data.Tcorr = Vehicles[n].VehParameter_Mixed_S[s].Tcorr;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Tcorr = 10 + ( ( ( ( rand() % 20 )/ 10.0 ) ) * 10 );
                        
                        veh_parameter_data.RT = Vehicles[n].VehParameter_Mixed_S[s].RT;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.RT = ( ( rand() % 300 )/ 10.0 ); // 0 --> 30: 1, 2 ...
                        
                        veh_parameter_data.Ao = Aotmp;
                        veh_parameter_data.So = Sotmp;
                        veh_parameter_data.Vo = Votmp;
                        veh_parameter_data.Amax = Amaxtmp;
                        veh_parameter_data.Wp = Wptmp;
                        
                        veh_parameter_data.DeltaW = 1 - veh_parameter_data.Wm;
                        veh_parameter_data.Sigma = ( ( 0.5 ) * ( 1 - veh_parameter_data.Gamma1 ) );
                        
                        Vehicles[n].VehParameterChildren_Mixed_S.push_back(veh_parameter_data);
                        NumberOfChildren = NumberOfChildren + 1;
                    }
                    
                    if (Random_CrossOver == 5)
                    {
                        //First Chromosome
                        VehParameterData veh_parameter_data;
                        
                        veh_parameter_data.Gamma1 = Vehicles[n].VehParameter_Mixed_S[s].Gamma1;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Gamma1 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
                        
                        veh_parameter_data.Gamma2 = Vehicles[n].VehParameter_Mixed_S[s].Gamma2;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Gamma2 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
                        
                        veh_parameter_data.PSI = Vehicles[n].VehParameter_Mixed_S[t].PSI;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.PSI = ( ( rand() % 100 ) / 10.0  );// 0 --> 10; 0.1, 0.2, 0.3, ....
                        
                        veh_parameter_data.Wm = Vehicles[n].VehParameter_Mixed_S[s].Wm;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Wm = ( ( rand() % 100 ) / 10.0  );// 0 --> 10; 0.1, 0.2, 0.3, ....
                        
                        veh_parameter_data.Wc = Vehicles[n].VehParameter_Mixed_S[s].Wc;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Wc = 50000 + ( ( ( rand() % 100 ) / 10.0  ) * 10000 );// 50 000 --> 150 000 ; 51000, 52000, 53000
                        
                        veh_parameter_data.Tmax = Vehicles[n].VehParameter_Mixed_S[s].Tmax;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Tmax = 1 + ((rand() % 100 )/10.0 ) ; // 1 --> 11; 1.1, 1.2, 1.3
                        
                        veh_parameter_data.Alpha = Vehicles[n].VehParameter_Mixed_S[s].Alpha;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Alpha = ( (rand() % 50 ) / 10.0 ) / 10.0;// 0 --> 5; 0.01, 0.02, 0.03 ....
                        
                        veh_parameter_data.Beta = Vehicles[n].VehParameter_Mixed_S[t].Beta;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Beta = 1 + ((rand() % 100 )/10.0 ) ; // 1 --> 11; 1.1, 1.2, 1.3
                        
                        veh_parameter_data.Tcorr = Vehicles[n].VehParameter_Mixed_S[t].Tcorr;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Tcorr = 10 + ( ( ( ( rand() % 20 )/ 10.0 ) ) * 10 );
                        
                        veh_parameter_data.RT = Vehicles[n].VehParameter_Mixed_S[t].RT;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.RT = ( ( rand() % 300 )/ 10.0 ); // 0 --> 30: 1, 2 ...
                        
                        veh_parameter_data.Ao = Aotmp;
                        veh_parameter_data.So = Sotmp;
                        veh_parameter_data.Vo = Votmp;
                        veh_parameter_data.Amax = Amaxtmp;
                        veh_parameter_data.Wp = Wptmp;
                        
                        veh_parameter_data.DeltaW = 1 - veh_parameter_data.Wm;
                        veh_parameter_data.Sigma = ( ( 0.5 ) * ( 1 - veh_parameter_data.Gamma1 ) );
                        
                        Vehicles[n].VehParameterChildren_Mixed_S.push_back(veh_parameter_data);
                        NumberOfChildren = NumberOfChildren + 1;
                        
                        //Second Chromosome
                        //VehParameterData veh_parameter_data;
                        
                        veh_parameter_data.Gamma1 = Vehicles[n].VehParameter_Mixed_S[t].Gamma1;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Gamma1 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
                        
                        veh_parameter_data.Gamma2 = Vehicles[n].VehParameter_Mixed_S[t].Gamma2;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Gamma2 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
                        
                        veh_parameter_data.PSI = Vehicles[n].VehParameter_Mixed_S[s].PSI;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.PSI = ( ( rand() % 100 ) / 10.0  );// 0 --> 10; 0.1, 0.2, 0.3, ....
                        
                        veh_parameter_data.Wm = Vehicles[n].VehParameter_Mixed_S[t].Wm;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Wm = ( ( rand() % 100 ) / 10.0  );// 0 --> 10; 0.1, 0.2, 0.3, ....
                        
                        veh_parameter_data.Wc = Vehicles[n].VehParameter_Mixed_S[t].Wc;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Wc = 50000 + ( ( ( rand() % 100 ) / 10.0  ) * 10000 );// 50 000 --> 150 000 ; 51000, 52000, 53000
                        
                        veh_parameter_data.Tmax = Vehicles[n].VehParameter_Mixed_S[t].Tmax;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Tmax = 1 + ((rand() % 100 )/10.0 ) ; // 1 --> 11; 1.1, 1.2, 1.3
                        
                        veh_parameter_data.Alpha = Vehicles[n].VehParameter_Mixed_S[t].Alpha;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Alpha = ( (rand() % 50 ) / 10.0 ) / 10.0;// 0 --> 5; 0.01, 0.02, 0.03 ....
                        
                        veh_parameter_data.Beta = Vehicles[n].VehParameter_Mixed_S[s].Beta;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Beta = 1 + ((rand() % 100 )/10.0 ) ; // 1 --> 11; 1.1, 1.2, 1.3
                        
                        veh_parameter_data.Tcorr = Vehicles[n].VehParameter_Mixed_S[s].Tcorr;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Tcorr = 10 + ( ( ( ( rand() % 20 )/ 10.0 ) ) * 10 );
                        
                        veh_parameter_data.RT = Vehicles[n].VehParameter_Mixed_S[s].RT;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.RT = ( ( rand() % 300 )/ 10.0 ); // 0 --> 30: 1, 2 ...
                        
                        veh_parameter_data.Ao = Aotmp;
                        veh_parameter_data.So = Sotmp;
                        veh_parameter_data.Vo = Votmp;
                        veh_parameter_data.Amax = Amaxtmp;
                        veh_parameter_data.Wp = Wptmp;
                        
                        veh_parameter_data.DeltaW = 1 - veh_parameter_data.Wm;
                        veh_parameter_data.Sigma = ( ( 0.5 ) * ( 1 - veh_parameter_data.Gamma1) );
                        
                        Vehicles[n].VehParameterChildren_Mixed_S.push_back(veh_parameter_data);
                        NumberOfChildren = NumberOfChildren + 1;
                    }
                    
                    if (Random_CrossOver == 6)
                    {
                        //First Chromosome
                        VehParameterData veh_parameter_data;
                        
                        veh_parameter_data.Gamma1 = Vehicles[n].VehParameter_Mixed_S[s].Gamma1;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Gamma1 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
                        
                        veh_parameter_data.Gamma2 = Vehicles[n].VehParameter_Mixed_S[s].Gamma2;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Gamma2 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
                        
                        veh_parameter_data.PSI = Vehicles[n].VehParameter_Mixed_S[t].PSI;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.PSI = ( ( rand() % 100 ) / 10.0  );// 0 --> 10; 0.1, 0.2, 0.3, ....
                        
                        veh_parameter_data.Wm = Vehicles[n].VehParameter_Mixed_S[s].Wm;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Wm = ( ( rand() % 100 ) / 10.0  );// 0 --> 10; 0.1, 0.2, 0.3, ....
                        
                        veh_parameter_data.Wc = Vehicles[n].VehParameter_Mixed_S[s].Wc;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Wc = 50000 + ( ( ( rand() % 100 ) / 10.0  ) * 10000 );// 50 000 --> 150 000 ; 51000, 52000, 53000
                        
                        veh_parameter_data.Tmax = Vehicles[n].VehParameter_Mixed_S[s].Tmax;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Tmax = 1 + ((rand() % 100 )/10.0 ) ; // 1 --> 11; 1.1, 1.2, 1.3
                        
                        veh_parameter_data.Alpha = Vehicles[n].VehParameter_Mixed_S[s].Alpha;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Alpha = ( (rand() % 50 ) / 10.0 ) / 10.0;// 0 --> 5; 0.01, 0.02, 0.03 ....
                        
                        veh_parameter_data.Beta = Vehicles[n].VehParameter_Mixed_S[s].Beta;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Beta = 1 + ((rand() % 100 )/10.0 ) ; // 1 --> 11; 1.1, 1.2, 1.3
                        
                        veh_parameter_data.Tcorr = Vehicles[n].VehParameter_Mixed_S[t].Tcorr;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Tcorr = 10 + ( ( ( ( rand() % 20 )/ 10.0 ) ) * 10 );
                        
                        veh_parameter_data.RT = Vehicles[n].VehParameter_Mixed_S[t].RT;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.RT = ( ( rand() % 300 )/ 10.0 ); // 0 --> 30: 1, 2 ...
                        
                        veh_parameter_data.Ao = Aotmp;
                        veh_parameter_data.So = Sotmp;
                        veh_parameter_data.Vo = Votmp;
                        veh_parameter_data.Amax = Amaxtmp;
                        veh_parameter_data.Wp = Wptmp;
                        
                        veh_parameter_data.DeltaW = 1 - veh_parameter_data.Wm;
                        veh_parameter_data.Sigma = ( ( 0.5 ) * ( 1 - veh_parameter_data.Gamma1 ) );
                        
                        Vehicles[n].VehParameterChildren_Mixed_S.push_back(veh_parameter_data);
                        NumberOfChildren = NumberOfChildren + 1;
                        
                        //Second Chromosome
                        //VehParameterData veh_parameter_data;
                        
                        veh_parameter_data.Gamma1 = Vehicles[n].VehParameter_Mixed_S[t].Gamma1;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Gamma1 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
                        
                        veh_parameter_data.Gamma2 = Vehicles[n].VehParameter_Mixed_S[t].Gamma2;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Gamma2 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
                        
                        veh_parameter_data.PSI = Vehicles[n].VehParameter_Mixed_S[s].PSI;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.PSI = ( ( rand() % 100 ) / 10.0  );// 0 --> 10; 0.1, 0.2, 0.3, ....
                        
                        veh_parameter_data.Wm = Vehicles[n].VehParameter_Mixed_S[t].Wm;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Wm = ( ( rand() % 100 ) / 10.0  );// 0 --> 10; 0.1, 0.2, 0.3, ....
                        
                        veh_parameter_data.Wc = Vehicles[n].VehParameter_Mixed_S[t].Wc;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Wc = 50000 + ( ( ( rand() % 100 ) / 10.0  ) * 10000 );// 50 000 --> 150 000 ; 51000, 52000, 53000
                        
                        veh_parameter_data.Tmax = Vehicles[n].VehParameter_Mixed_S[t].Tmax;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Tmax = 1 + ((rand() % 100 )/10.0 ) ; // 1 --> 11; 1.1, 1.2, 1.3
                        
                        veh_parameter_data.Alpha = Vehicles[n].VehParameter_Mixed_S[t].Alpha;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Alpha = ( (rand() % 50 ) / 10.0 ) / 10.0;// 0 --> 5; 0.01, 0.02, 0.03 ....
                        
                        veh_parameter_data.Beta = Vehicles[n].VehParameter_Mixed_S[t].Beta;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Beta = 1 + ((rand() % 100 )/10.0 ) ; // 1 --> 11; 1.1, 1.2, 1.3
                        
                        veh_parameter_data.Tcorr = Vehicles[n].VehParameter_Mixed_S[s].Tcorr;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Tcorr = 10 + ( ( ( ( rand() % 20 )/ 10.0 ) ) * 10 );
                        
                        veh_parameter_data.RT = Vehicles[n].VehParameter_Mixed_S[s].RT;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.RT = ( ( rand() % 300 )/ 10.0 ); // 0 --> 30: 1, 2 ...
                        
                        veh_parameter_data.Ao = Aotmp;
                        veh_parameter_data.So = Sotmp;
                        veh_parameter_data.Vo = Votmp;
                        veh_parameter_data.Amax = Amaxtmp;
                        veh_parameter_data.Wp = Wptmp;
                        
                        veh_parameter_data.DeltaW = 1 - veh_parameter_data.Wm;
                        veh_parameter_data.Sigma = ( ( 0.5 ) * ( 1 - veh_parameter_data.Gamma1 ) );
                        
                        Vehicles[n].VehParameterChildren_Mixed_S.push_back(veh_parameter_data);
                        NumberOfChildren = NumberOfChildren + 1;
                    }
                    
                    if (Random_CrossOver == 7)
                    {
                        //First Chromosome
                        VehParameterData veh_parameter_data;
                        
                        veh_parameter_data.Gamma1 = Vehicles[n].VehParameter_Mixed_S[s].Gamma1;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Gamma1 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
                        
                        veh_parameter_data.Gamma2 = Vehicles[n].VehParameter_Mixed_S[s].Gamma2;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Gamma2 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
                        
                        veh_parameter_data.PSI = Vehicles[n].VehParameter_Mixed_S[t].PSI;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.PSI = ( ( rand() % 100 ) / 10.0  );// 0 --> 10; 0.1, 0.2, 0.3, ....
                        
                        veh_parameter_data.Wm = Vehicles[n].VehParameter_Mixed_S[s].Wm;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Wm = ( ( rand() % 100 ) / 10.0  );// 0 --> 10; 0.1, 0.2, 0.3, ....
                        
                        veh_parameter_data.Wc = Vehicles[n].VehParameter_Mixed_S[s].Wc;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Wc = 50000 + ( ( ( rand() % 100 ) / 10.0  ) * 10000 );// 50 000 --> 150 000 ; 51000, 52000, 53000
                        
                        veh_parameter_data.Tmax = Vehicles[n].VehParameter_Mixed_S[s].Tmax;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Tmax = 1 + ((rand() % 100 )/10.0 ) ; // 1 --> 11; 1.1, 1.2, 1.3
                        
                        veh_parameter_data.Alpha = Vehicles[n].VehParameter_Mixed_S[s].Alpha;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Alpha = ( (rand() % 50 ) / 10.0 ) / 10.0;// 0 --> 5; 0.01, 0.02, 0.03 ....
                        
                        veh_parameter_data.Beta = Vehicles[n].VehParameter_Mixed_S[s].Beta;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Beta = 1 + ((rand() % 100 )/10.0 ) ; // 1 --> 11; 1.1, 1.2, 1.3
                        
                        veh_parameter_data.Tcorr = Vehicles[n].VehParameter_Mixed_S[s].Tcorr;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Tcorr = 10 + ( ( ( ( rand() % 20 )/ 10.0 ) ) * 10 );
                        
                        veh_parameter_data.RT = Vehicles[n].VehParameter_Mixed_S[t].RT;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.RT = ( ( rand() % 300 )/ 10.0 ); // 0 --> 30: 1, 2 ...
                        
                        veh_parameter_data.Ao = Aotmp;
                        veh_parameter_data.So = Sotmp;
                        veh_parameter_data.Vo = Votmp;
                        veh_parameter_data.Amax = Amaxtmp;
                        veh_parameter_data.Wp = Wptmp;
                        
                        veh_parameter_data.DeltaW = 1 - veh_parameter_data.Wm;
                        veh_parameter_data.Sigma = ( ( 0.5 ) * ( 1 - veh_parameter_data.Gamma1 ) );
                        
                        Vehicles[n].VehParameterChildren_Mixed_S.push_back(veh_parameter_data);
                        NumberOfChildren = NumberOfChildren + 1;
                        
                        //Second Chromosome
                        //VehParameterData veh_parameter_data;
                        
                        veh_parameter_data.Gamma1 = Vehicles[n].VehParameter_Mixed_S[t].Gamma1;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Gamma1 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
                        
                        veh_parameter_data.Gamma2 = Vehicles[n].VehParameter_Mixed_S[t].Gamma2;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Gamma2 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
                        
                        veh_parameter_data.PSI = Vehicles[n].VehParameter_Mixed_S[s].PSI;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.PSI = ( ( rand() % 100 ) / 10.0  );// 0 --> 10; 0.1, 0.2, 0.3, ....
                        
                        veh_parameter_data.Wm = Vehicles[n].VehParameter_Mixed_S[t].Wm;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Wm = ( ( rand() % 100 ) / 10.0  );// 0 --> 10; 0.1, 0.2, 0.3, ....
                        
                        veh_parameter_data.Wc = Vehicles[n].VehParameter_Mixed_S[t].Wc;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Wc = 50000 + ( ( ( rand() % 100 ) / 10.0  ) * 10000 );// 50 000 --> 150 000 ; 51000, 52000, 53000
                        
                        veh_parameter_data.Tmax = Vehicles[n].VehParameter_Mixed_S[t].Tmax;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Tmax = 1 + ((rand() % 100 )/10.0 ) ; // 1 --> 11; 1.1, 1.2, 1.3
                        
                        veh_parameter_data.Alpha = Vehicles[n].VehParameter_Mixed_S[t].Alpha;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Alpha = ( (rand() % 50 ) / 10.0 ) / 10.0;// 0 --> 5; 0.01, 0.02, 0.03 ....
                        
                        veh_parameter_data.Beta = Vehicles[n].VehParameter_Mixed_S[t].Beta;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Beta = 1 + ((rand() % 100 )/10.0 ) ; // 1 --> 11; 1.1, 1.2, 1.3
                        
                        veh_parameter_data.Tcorr = Vehicles[n].VehParameter_Mixed_S[t].Tcorr;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Tcorr = 10 + ( ( ( ( rand() % 20 )/ 10.0 ) ) * 10 );
                        
                        veh_parameter_data.RT = Vehicles[n].VehParameter_Mixed_S[s].RT;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.RT = ( ( rand() % 300 )/ 10.0 ); // 0 --> 30: 1, 2 ...
                        
                        veh_parameter_data.Ao = Aotmp;
                        veh_parameter_data.So = Sotmp;
                        veh_parameter_data.Vo = Votmp;
                        veh_parameter_data.Amax = Amaxtmp;
                        veh_parameter_data.Wp = Wptmp;
                        
                        veh_parameter_data.DeltaW = 1 - veh_parameter_data.Wm;
                        veh_parameter_data.Sigma = ( ( 0.5 ) * ( 1 - veh_parameter_data.Gamma1 ) );
                        
                        Vehicles[n].VehParameterChildren_Mixed_S.push_back(veh_parameter_data);
                        NumberOfChildren = NumberOfChildren + 1;
                    }
                    
                    if (Random_CrossOver == 8)
                    {
                        //First Chromosome
                        VehParameterData veh_parameter_data;
                        
                        veh_parameter_data.Gamma1 = Vehicles[n].VehParameter_Mixed_S[s].Gamma1;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Gamma1 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
                        
                        veh_parameter_data.Gamma2 = Vehicles[n].VehParameter_Mixed_S[s].Gamma2;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Gamma2 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
                        
                        veh_parameter_data.PSI = Vehicles[n].VehParameter_Mixed_S[t].PSI;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.PSI = ( ( rand() % 100 ) / 10.0  );// 0 --> 10; 0.1, 0.2, 0.3, ....
                        
                        veh_parameter_data.Wm = Vehicles[n].VehParameter_Mixed_S[s].Wm;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Wm = ( ( rand() % 100 ) / 10.0  );// 0 --> 10; 0.1, 0.2, 0.3, ....
                        
                        veh_parameter_data.Wc = Vehicles[n].VehParameter_Mixed_S[s].Wc;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Wc = 50000 + ( ( ( rand() % 100 ) / 10.0  ) * 10000 );// 50 000 --> 150 000 ; 51000, 52000, 53000
                        
                        veh_parameter_data.Tmax = Vehicles[n].VehParameter_Mixed_S[s].Tmax;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Tmax = 1 + ((rand() % 100 )/10.0 ) ; // 1 --> 11; 1.1, 1.2, 1.3
                        
                        veh_parameter_data.Alpha = Vehicles[n].VehParameter_Mixed_S[s].Alpha;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Alpha = ( (rand() % 50 ) / 10.0 ) / 10.0;// 0 --> 5; 0.01, 0.02, 0.03 ....
                        
                        veh_parameter_data.Beta = Vehicles[n].VehParameter_Mixed_S[s].Beta;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Beta = 1 + ((rand() % 100 )/10.0 ) ; // 1 --> 11; 1.1, 1.2, 1.3
                        
                        veh_parameter_data.Tcorr = Vehicles[n].VehParameter_Mixed_S[s].Tcorr;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Tcorr = 10 + ( ( ( ( rand() % 20 )/ 10.0 ) ) * 10 );
                        
                        veh_parameter_data.RT = Vehicles[n].VehParameter_Mixed_S[s].RT;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.RT = ( ( rand() % 300 )/ 10.0 ); // 0 --> 30: 1, 2 ...
                        
                        veh_parameter_data.Ao = Aotmp;
                        veh_parameter_data.So = Sotmp;
                        veh_parameter_data.Vo = Votmp;
                        veh_parameter_data.Amax = Amaxtmp;
                        veh_parameter_data.Wp = Wptmp;
                        
                        veh_parameter_data.DeltaW = 1 - veh_parameter_data.Wm;
                        veh_parameter_data.Sigma = ( ( 0.5 ) * ( 1 - veh_parameter_data.Gamma1 ) );
                        
                        Vehicles[n].VehParameterChildren_Mixed_S.push_back(veh_parameter_data);
                        NumberOfChildren = NumberOfChildren + 1;
                        
                        //Second Chromosome
                        //VehParameterData veh_parameter_data;
                        
                        veh_parameter_data.Gamma1 = Vehicles[n].VehParameter_Mixed_S[t].Gamma1;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Gamma1 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
                        
                        veh_parameter_data.Gamma2 = Vehicles[n].VehParameter_Mixed_S[t].Gamma2;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Gamma2 = ( ( rand() % 20 )/ 10.0 ); // 0 --> 2: 0.1, 0.2 ....
                        
                        veh_parameter_data.PSI = Vehicles[n].VehParameter_Mixed_S[s].PSI;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.PSI = ( ( rand() % 100 ) / 10.0  );// 0 --> 10; 0.1, 0.2, 0.3, ....
                        
                        veh_parameter_data.Wm = Vehicles[n].VehParameter_Mixed_S[t].Wm;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Wm = ( ( rand() % 100 ) / 10.0  );// 0 --> 10; 0.1, 0.2, 0.3, ....
                        
                        veh_parameter_data.Wc = Vehicles[n].VehParameter_Mixed_S[t].Wc;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Wc = 50000 + ( ( ( rand() % 100 ) / 10.0  ) * 10000 );// 50 000 --> 150 000 ; 51000, 52000, 53000
                        
                        veh_parameter_data.Tmax = Vehicles[n].VehParameter_Mixed_S[t].Tmax;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Tmax = 1 + ((rand() % 100 )/10.0 ) ; // 1 --> 11; 1.1, 1.2, 1.3
                        
                        veh_parameter_data.Alpha = Vehicles[n].VehParameter_Mixed_S[t].Alpha;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Alpha = ( (rand() % 50 ) / 10.0 ) / 10.0;// 0 --> 5; 0.01, 0.02, 0.03 ....
                        
                        veh_parameter_data.Beta = Vehicles[n].VehParameter_Mixed_S[t].Beta;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Beta = 1 + ((rand() % 100 )/10.0 ) ; // 1 --> 11; 1.1, 1.2, 1.3
                        
                        veh_parameter_data.Tcorr = Vehicles[n].VehParameter_Mixed_S[t].Tcorr;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.Tcorr = 10 + ( ( ( ( rand() % 20 )/ 10.0 ) ) * 10 );
                        
                        veh_parameter_data.RT = Vehicles[n].VehParameter_Mixed_S[t].RT;
                        Random_Mutation = (( rand ()% 1000 / 10));
                        if (Random_Mutation <=10)
                            veh_parameter_data.RT = ( ( rand() % 300 )/ 10.0 ); // 0 --> 30: 1, 2 ...
                        
                        veh_parameter_data.Ao = Aotmp;
                        veh_parameter_data.So = Sotmp;
                        veh_parameter_data.Vo = Votmp;
                        veh_parameter_data.Amax = Amaxtmp;
                        veh_parameter_data.Wp = Wptmp;
                        
                        veh_parameter_data.DeltaW = 1 - veh_parameter_data.Wm;
                        veh_parameter_data.Sigma = ( ( 0.5 ) * ( 1 - veh_parameter_data.Gamma1 ) );
                        
                        Vehicles[n].VehParameterChildren_Mixed_S.push_back(veh_parameter_data);
                        NumberOfChildren = NumberOfChildren + 1;
                    }
                }
            }
            
            //Compute Measures of Effectiveness for different chromosomes
            
            for (int i = 0; i < ( NumberOfChildren - 1) ; i++)
            {
                Yt = 0.0;
                
                cout<<n<<"        "<<i<<"        "<<Error_Counter<<endl;
                /*if (n == 3 && i == 3)
                 {
                 cout<<"stop"<<endl;
                 getch();
                 }*/
                
                Vehicles[n].VehParameterChildren_Mixed_S[i].Accident = 0;
                
                Vehicles[n].VehParameterChildren_Mixed_S[i].AbsoluteError_Velocity = 0.0;
                Vehicles[n].VehParameterChildren_Mixed_S[i].RelativeError_Velocity = 0.0;
                Vehicles[n].VehParameterChildren_Mixed_S[i].MixedError_Velocity = 0.0;
                
                Vehicles[n].VehParameterChildren_Mixed_S[i].AbsoluteError_Gap = 0.0;
                Vehicles[n].VehParameterChildren_Mixed_S[i].RelativeError_Gap = 0.0;
                Vehicles[n].VehParameterChildren_Mixed_S[i].MixedError_Gap = 0.0;
                
                Vehicles[n].VehParameterChildren_Mixed_S[i].AbsoluteError_RelativeVelocity = 0.0;
                Vehicles[n].VehParameterChildren_Mixed_S[i].RelativeError_RelativeVelocity = 0.0;
                Vehicles[n].VehParameterChildren_Mixed_S[i].MixedError_RelativeVelocity = 0.0;
                
                
                ErrorDummy1_V = 0.0;
                ErrorDummy2_V = 0.0;
                ErrorDummy3_V = 0.0;
                ErrorDummy4_V = 0.0;
                ErrorDummy5_V = 0.0;
                ErrorDummy1_V_Validation = 0.0;
                ErrorDummy2_V_Validation = 0.0;
                ErrorDummy3_V_Validation = 0.0;
                ErrorDummy4_V_Validation = 0.0;
                ErrorDummy5_V_Validation = 0.0;
                
                ErrorDummy1_S = 0.0;
                ErrorDummy2_S = 0.0;
                ErrorDummy3_S = 0.0;
                ErrorDummy4_S = 0.0;
                ErrorDummy5_S = 0.0;
                ErrorDummy1_S_Validation = 0.0;
                ErrorDummy2_S_Validation = 0.0;
                ErrorDummy3_S_Validation = 0.0;
                ErrorDummy4_S_Validation = 0.0;
                ErrorDummy5_S_Validation = 0.0;
                
                ErrorDummy1_DV = 0.0;
                ErrorDummy2_DV = 0.0;
                ErrorDummy3_DV = 0.0;
                ErrorDummy4_DV = 0.0;
                ErrorDummy5_DV = 0.0;
                ErrorDummy1_DV_Validation = 0.0;
                ErrorDummy2_DV_Validation = 0.0;
                ErrorDummy3_DV_Validation = 0.0;
                ErrorDummy4_DV_Validation = 0.0;
                ErrorDummy5_DV_Validation = 0.0;
                
                //Test: Attempt to Reset Values
                
                double DummyA;
                double DummyV;
                double DummyX;
                double DummyDX;
                double DummyDV;
                double DummyDS;
                
                Leader_Previous = 9999;
                Lane_Previous = 99;
                LeadersChangingLanestmp = 0;
                
                
                RT_Update = Vehicles[n].VehParameterChildren_Mixed_S[i].RT;
                RT_Counter = 0;
                
                Gamma1_D = Vehicles[n].VehParameterChildren_Mixed_S[i].Gamma1;
                Gamma2_D = Vehicles[n].VehParameterChildren_Mixed_S[i].Gamma2;
                PSI_D = Vehicles[n].VehParameterChildren_Mixed_S[i].PSI;
                Wm_D = Vehicles[n].VehParameterChildren_Mixed_S[i].Wm;
                Wc_D = Vehicles[n].VehParameterChildren_Mixed_S[i].Wc;
                Tmax_D = Vehicles[n].VehParameterChildren_Mixed_S[i].Tmax;
                Alpha_D = Vehicles[n].VehParameterChildren_Mixed_S[i].Alpha;
                Beta_D = Vehicles[n].VehParameterChildren_Mixed_S[i].Beta;
                Tcorr_D = Vehicles[n].VehParameterChildren_Mixed_S[i].Tcorr;
                
                Ao_D = Vehicles[n].VehParameterChildren_Mixed_S[i].Ao;
                So_D = Vehicles[n].VehParameterChildren_Mixed_S[i].So;
                Vo_D = Vehicles[n].VehParameterChildren_Mixed_S[i].Vo;
                Amax_D = Vehicles[n].VehParameterChildren_Mixed_S[i].Amax;
                
                Wp_D = Vehicles[n].VehParameterChildren_Mixed_S[i].Wp;
                DeltaW_D = Vehicles[n].VehParameterChildren_Mixed_S[i].DeltaW;
                Sigma_D = Vehicles[n].VehParameterChildren_Mixed_S[i].Sigma;
                
                for (int f = 1; f < Vehicles[n].NumFrames - 1 ; f++) //There is a need for the next frame for computing the corresponding data measure to the simulated measure
                {
                    //Added by Samer Hamdar: 14 October 2008
                    if ( Vehicles[n].VehFrames[f].Mode_Previous != Vehicles[n].VehFrames[f].Mode || f == 0)
                        Vehicles[n].VehFrames[f].DurationTime_Frame = 0;
                    else
                        Vehicles[n].VehFrames[f].DurationTime_Frame = Vehicles[n].VehFrames[f-1].DurationTime_Frame + 1;
                    
                    Vehicles[n].VehFrames[f].Mode_Previous = Vehicles[n].VehFrames[f].Mode;
                    //End of Added by Samer Hamdar: 14 October 2008
                    
                    
                    if (Vehicles[n].VehParameterChildren_Mixed_S[i].Accident == 0)
                    {
                        //__________________________________________________________________________________________________________
                        if ( Vehicles[n].VehFrames[f].PreVehID  != 0.0)
                        {
                            //**************Computing Wallsten Measures and Error Terms Increment
                            
                            if ( RT_Update == 0 )
                            {
                                //Testing Parameters for Watch
                                DummyA = Vehicles[n].VehFrames[f].VehicleAcceleration;
                                DummyV = Vehicles[n].VehFrames[f].VehicleVelocity;
                                DummyX = Vehicles[n].VehFrames[f].LocalY;
                                DummyDX = Vehicles[n].VehFrames[f].SpaceHeadway;
                                DummyDV = Vehicles[n].VehFrames[f].RelativeVelocity;
                                DummyDS = Vehicles[n].VehFrames[f].SpaceGap;
                                
                                if (Vehicles[n].VehFrames[f].PreVehID != Leader_Previous)
                                {
                                    //Removed by Samer Hamdar: 8 October 2008
                                    //At_Update = Vehicles[n].VehFrames[f].VehicleAcceleration;
                                    //Vt_Update = Vehicles[n].VehFrames[f].VehicleVelocity;
                                    //Xt_Update = Vehicles[n].VehFrames[f].LocalY;
                                    //DeltaX_Update = Vehicles[n].VehFrames[f].SpaceHeadway;
                                    //DeltaV_Update = Vehicles[n].VehFrames[f].RelativeVelocity;
                                    //S_Update = Vehicles[n].VehFrames[f].SpaceGap;
                                    //End of Removed by Samer Hamdar: 8 October 2008
                                    
                                    RT_Counter = 0;
                                    
                                    //Added by Samer Hamdar: 8 October 2008
                                    
                                    //Changed Lanes
                                    if (Vehicles[n].VehFrames[f].Lane != Lane_Previous)
                                    {
                                        At_Update = Vehicles[n].VehFrames[f].VehicleAcceleration;
                                        Vt_Update = Vehicles[n].VehFrames[f].VehicleVelocity;
                                        Xt_Update = Vehicles[n].VehFrames[f].LocalY;
                                        DeltaX_Update = Vehicles[n].VehFrames[f].SpaceHeadway;
                                        DeltaV_Update = Vehicles[n].VehFrames[f].RelativeVelocity;
                                        S_Update = Vehicles[n].VehFrames[f].SpaceGap;
                                        
                                        //New Episode:
                                        if (f != 0)//Not First Reading
                                        {
                                            Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame = 0;
                                            Vehicles[n].VehFrames[f].FirstEpisodeDummy_Frame = 0;
                                            Vehicles[n].VehFrames[f].DurationTime_Frame = 0;
                                        }
                                        
                                        //Store Lane Changing Hazard Struct
                                        Vehicles[n].NumberOfLaneChanges = Vehicles[n].NumberOfLaneChanges + 1;
                                        LaneChangingData lane_changing_data;
                                        lane_changing_data.LaneChangingTime = Vehicles[n].VehFrames[f].FrameID;
                                        
                                        if (Vehicles[n].VehFrames[f].Mode == 1 )
                                        {
                                            lane_changing_data.LaneChangingHazard = Vehicles[n].VehFrames[f - 1].LaneChange_Hazard_CF;
                                            lane_changing_data.CompetingHazard = Vehicles[n].VehFrames[f - 1].Mode_Hazard_CF;
                                            lane_changing_data.Mode = Vehicles[n].VehFrames[f].Mode;
                                        }
                                        else if (Vehicles[n].VehFrames[f].Mode == 0)
                                        {
                                            lane_changing_data.LaneChangingHazard = Vehicles[n].VehFrames[f - 1].LaneChange_Hazard_FF;
                                            lane_changing_data.CompetingHazard = Vehicles[n].VehFrames[f - 1].Mode_Hazard_FF;
                                            lane_changing_data.Mode = Vehicles[n].VehFrames[f].Mode;
                                        }
                                        
                                        Vehicles[n].LaneChanging.push_back(lane_changing_data);
                                        
                                    }
                                    else //Did not Change Lanes
                                    {
                                        At_Update = Vehicles[n].VehFrames[f-1].WallstenAcceleration;//Since at this step, no Wallsten Acceleration has been created --> taken from previous time step
                                        Vt_Update = Vehicles[n].VehFrames[f-1].WallstenVelocity;
                                        Xt_Update = Vehicles[n].VehFrames[f-1].WallstenX;
                                        
                                        //NB: DOUBT
                                        DeltaX_Update = Vehicles[n].VehFrames[f].SpaceHeadway + ( Vehicles[n].VehFrames[f].LocalY - Xt_Update );
                                        DeltaV_Update = Vehicles[n].VehFrames[f].RelativeVelocity + ( Vt_Update - Vehicles[n].VehFrames[f].VehicleVelocity );
                                        S_Update = Vehicles[n].VehFrames[f].SpaceGap + ( Vehicles[n].VehFrames[f].LocalY - Xt_Update );//Possible Error: Problem Since Space Gaps are Sometimes Negative: Use Space Headway Instead
                                        
                                        if (f != 0)//Not First Reading
                                            Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame = Vehicles[n].VehFrames[f - 1].LeadersChangingLanes_Frame + 1;
                                        
                                    }
                                    //End of Added by Samer Hamdar: 8 October 2008
                                }
                                else
                                {
                                    At_Update = Vehicles[n].VehFrames[f-1].WallstenAcceleration;//Since at this step, no Wallsten Acceleration has been created --> taken from previous time step
                                    Vt_Update = Vehicles[n].VehFrames[f-1].WallstenVelocity;
                                    Xt_Update = Vehicles[n].VehFrames[f-1].WallstenX;
                                    DeltaX_Update = Vehicles[n].VehFrames[f-1].WallstenSpaceHeadway;
                                    DeltaV_Update = Vehicles[n].VehFrames[f-1].WallstenRelativeSpeed;
                                    S_Update = Vehicles[n].VehFrames[f-1].WallstenGap;
                                }
                                
                                //Test: Seff: Martin's Version: Keep for Updating Purposes: Keep Errors Defined
                                //if ( ( S_Update - So_D ) > 0.0 )
                                if ( ( S_Update - So_D ) > 0.1 )
                                    Seff = ( S_Update - So_D );
                                else
                                    Seff = 0.1;
                                //Seff = 0.0;
                                
                                
                                //Seff: Modified Conservative Version: Minimum Seff = S0 = 3 and Use Space Headway Instead of Gap
                                //if ( ( DeltaX_Update - So_D ) > 3.0 )
                                //    Seff = ( DeltaX_Update - So_D );
                                //else
                                //    Seff = 3.0;
                                
                                ////Test
                                //if (DeltaV_Update == 0.0)
                                //    DeltaV_Update = 0.1;
                                
                                //Tau
                                if ( DeltaV_Update > ( Seff / Tmax_D ) )
                                    Tau =  ( Seff / DeltaV_Update );
                                else
                                    Tau = Tmax_D;
                                
                                ////Test
                                //if (Vt_Update < 0.1)
                                //    Vt_Update = 0.1;
                                
                                //Zprime
                                Zprime = ( Tau / ( 2.0 * Alpha_D * Vt_Update ) );
                                
                                //Zdoubleprime
                                Zdoubleprime = 0.0;
                                
                                
                                //Zstar
                                Zstar = ( - sqrt ( 2.0 * log ( ( Ao_D * Wc_D * Zprime ) / ( sqrt ( 2.0 * PI ) ) ) ) );
                                
                                //Astar : Initial Estimate
                                Astar = ( ( 2.0 / Tau) * ( ( Seff / Tau) - DeltaV_Update + ( Alpha_D * Vt_Update * Zstar ) ) );
                                
                                //Newton's Method loop
                                for ( int NewtonCounter = 0; NewtonCounter < 4 ; NewtonCounter++ )
                                {
                                    X = ( Astar / Ao_D );
                                    
                                    /*G = ( X / ( pow ( ( 1 + pow ( X , 2.0 ) ) , Sigma_D ) ) );
                                    
                                    Gprime = ( ( 1.0 / ( pow ( ( 1.0 + pow ( X , 2.0 )), Gamma_D ) ) ) - ( ( 2.0 * Gamma_D * pow ( X , 2.0 ) ) / ( pow ( ( 1 + pow ( X , 2.0 ) ) ,( Gamma_D + 1.0) ) ) ) );
                                    Gdoubleprime = ( ( ( - 6.0 * Gamma_D * X ) / ( pow ( ( 1.0 + pow ( X , 2.0 ) ) ,( Gamma_D + 1) ) ) ) + ( ( 4.0 * Gamma_D * ( Gamma_D + 1 ) * pow ( X , 3.0) ) / ( pow ( ( 1 + pow ( X , 2.0 ) ) ,( Gamma_D + 2.0) ) ) ) );
                                    
                                    Uptprime = ( ( 1.0 / Ao_D ) *  ( ( ( Wm_D + ( ( DeltaW_D * ( 1 + tanh ( X ) ) ) / PSI_D ) ) * ( Gprime) ) + ( ( DeltaW_D * G ) / ( PSI_D * pow ( cosh ( X ) , 2 ) ) ) ) );
                                    Uptdoubleprime = ( ( 1.0 / ( pow (Ao_D , 2.0 ) ) ) *  ( ( ( Wm_D + (  (  DeltaW_D * ( 1 + tanh ( X ) ) ) / PSI_D ) ) * ( Gdoubleprime) ) + ( ( ( DeltaW_D ) / ( PSI_D * pow ( cosh ( X ) , 2.0 ) ) ) * ( Gprime - ( tanh ( X ) * G ) ) ) ) );
                                    */
                                    
                                    if (X >= 0)
                                    {
                                        Uptprime = Gamma1_D * pow(X, Gamma1_D - 1);
                                        Uptdoubleprime = Gamma1_D * (Gamma1_D - 1) * pow(X, Gamma1_D - 2);
                                    }
                                    else
                                    {
                                        Uptprime = Wm_D * Gamma2_D * pow(-X, Gamma2_D - 1);
                                        Uptdoubleprime = -Wm_D * Gamma2_D * (Gamma2_D - 1) * pow(-X, Gamma2_D - 2);
                                    }
                                    
                                    /*if (Astar >= 0)
                                    {
                                        Uptprime = pow(Astar, -Gamma1_D);
                                        Uptdoubleprime = -Gamma1_D * pow(Astar, -Gamma1_D - 1);
                                    }
                                    else
                                    {
                                        Uptprime = pow(-Astar, -Gamma2_D);
                                        Uptdoubleprime = Gamma2_D * pow(-Astar, -Gamma2_D - 1);
                                    }*/
                                    
                                    /*if (Astar >= 0)
                                    {
                                        Uptprime = -(1 - Gamma1_D) * pow(-Astar, -Gamma1_D) * exp(-Wm_D * pow(-Astar, 1 - Gamma1_D));
                                        Uptdoubleprime = -(1 - Gamma1_D) * -Gamma1_D * pow(-Astar, -Gamma1_D - 1) * exp(-Wm_D * pow(-Astar, 1 - Gamma1_D)) -
                                            pow(((1 - Gamma1_D) * pow(-Astar, -Gamma1_D)), 2) * exp(-Wm_D * pow(-Astar, 1 - Gamma1_D));
                                    }
                                    else
                                    {
                                        Uptprime = (1 - Gamma2_D) * pow(-Astar, -Gamma2_D) * exp(-Wm_D * pow(-Astar, 1 - Gamma2_D));
                                        Uptdoubleprime = (1 - Gamma1_D) * -Gamma1_D * pow(-Astar, -Gamma1_D - 1) * exp(-Wm_D * pow(-Astar, 1 - Gamma1_D)) +
                                            pow(((1 - Gamma1_D) * pow(-Astar, -Gamma1_D)), 2) * exp(-Wm_D * pow(-Astar, 1 - Gamma1_D));
                                    }*/
                                    
                                    Z = ( ( DeltaV_Update + ( ( 0.5 ) * Astar * Tau ) - ( Seff / Tau) ) / ( Alpha_D * Vt_Update ) );
                                    fn = ( 1.0 / sqrt ( 2 * PI) ) * exp (  - pow ( Z , 2.0) / 2.0 );
                                    
                                    F = Uptprime - Wc_D * fn * Zprime;
                                    Fprime = Uptdoubleprime + Wc_D * fn * ( Z * pow ( Zprime, 2.0) + Zdoubleprime );
                                    
                                    Astar = Astar - ( F / Fprime);
                                }
                                
                                //Compute U'' or F' for the last time for variance computation
                                X = ( Astar / Ao_D );
                                
                                /*G = ( X / ( pow ( ( 1.0 + pow ( X , 2.0 ) ) , Sigma_D ) ) );
                                
                                Gprime = ( ( 1.0 / ( pow ( ( 1 + pow ( X , 2.0 )), Gamma_D ) ) ) - ( ( 2.0 * Gamma_D * pow ( X , 2.0 ) ) / ( pow ( ( 1 + pow ( X , 2.0 ) ) ,( Gamma_D + 1.0) ) ) ) );
                                Gdoubleprime = ( ( ( - 6.0 * Gamma_D * X ) / ( pow ( ( 1.0 + pow ( X , 2.0 ) ) ,( Gamma_D + 1.0) ) ) ) + ( ( 4.0 * Gamma_D * ( Gamma_D + 1.0 ) * pow ( X , 3.0) ) / ( pow ( ( 1.0 + pow ( X , 2.0 ) ) ,( Gamma_D + 2.0) ) ) ) );
                                
                                Uptprime = ( ( 1.0 / Ao_D ) *  ( ( ( Wm_D + ( ( DeltaW_D * ( 1 + tanh ( X ) ) ) / PSI_D ) ) * ( Gprime) ) + ( ( DeltaW_D * G ) / ( PSI_D * pow ( cosh ( X ) , 2 ) ) ) ) );
                                Uptdoubleprime = ( ( 1.0 / ( pow (Ao_D , 2.0 ) ) ) *  ( ( ( Wm_D + (  (  DeltaW_D * ( 1 + tanh ( X ) ) ) / PSI_D ) ) * ( Gdoubleprime) ) + ( ( ( DeltaW_D ) / ( PSI_D * pow ( cosh ( X ) , 2.0 ) ) ) * ( Gprime - ( tanh ( X ) * G ) ) ) ) );
                                */
                                
                                if (X >= 0)
                                 {
                                 Uptprime = Gamma1_D * pow(X, Gamma1_D - 1);
                                 Uptdoubleprime = Gamma1_D * (Gamma1_D - 1) * pow(X, Gamma1_D - 2);
                                 }
                                 else
                                 {
                                 Uptprime = Wm_D * Gamma2_D * pow(-X, Gamma2_D - 1);
                                 Uptdoubleprime = -Wm_D * Gamma2_D * (Gamma2_D - 1) * pow(-X, Gamma2_D - 2);
                                 }
                                
                                /*if (Astar >= 0)
                                {
                                    Uptprime = pow(Astar, -Gamma1_D);
                                    Uptdoubleprime = -Gamma1_D * pow(Astar, -Gamma1_D - 1);
                                }
                                else
                                {
                                    Uptprime = pow(-Astar, -Gamma2_D);
                                    Uptdoubleprime = Gamma2_D * pow(-Astar, -Gamma2_D - 1);
                                }*/
                                
                                /*if (Astar >= 0)
                                 {
                                 Uptprime = -(1 - Gamma1_D) * pow(-Astar, -Gamma1_D) * exp(-Wm_D * pow(-Astar, 1 - Gamma1_D));
                                 Uptdoubleprime = -(1 - Gamma1_D) * -Gamma1_D * pow(-Astar, -Gamma1_D - 1) * exp(-Wm_D * pow(-Astar, 1 - Gamma1_D)) -
                                 pow(((1 - Gamma1_D) * pow(-Astar, -Gamma1_D)), 2) * exp(-Wm_D * pow(-Astar, 1 - Gamma1_D));
                                 }
                                 else
                                 {
                                 Uptprime = (1 - Gamma2_D) * pow(-Astar, -Gamma2_D) * exp(-Wm_D * pow(-Astar, 1 - Gamma2_D));
                                 Uptdoubleprime = (1 - Gamma1_D) * -Gamma1_D * pow(-Astar, -Gamma1_D - 1) * exp(-Wm_D * pow(-Astar, 1 - Gamma1_D)) +
                                 pow(((1 - Gamma1_D) * pow(-Astar, -Gamma1_D)), 2) * exp(-Wm_D * pow(-Astar, 1 - Gamma1_D));
                                 }*/
                                
                                
                                
                                Z = ( ( DeltaV_Update + ( ( 0.5 ) * Astar * Tau ) - ( Seff / Tau) ) / ( Alpha_D * Vt_Update ) );
                                fn = ( 1.0 / sqrt ( 2 * PI ) ) * exp (  - pow ( Z , 2.0) / 2.0 );
                                
                                F = Uptprime - Wc_D * fn * Zprime;
                                Fprime = Uptdoubleprime + Wc_D * fn * ( Z * pow ( Zprime, 2.0) + Zdoubleprime );
                                
                                
                                //Var (Variance)
                                
                                Var = -1.0 / ( Beta_D * Fprime );
                                
                                //Wiener Process (Problem?)
                                
                                Random_Wiener = ( (rand()%1000) / 1000.0 );
                                Yt_previous = Yt;
                                
                                Yt = Yt * exp ( - DeltaT / Tau) + sqrt ( 24.0 * DeltaT / Tau ) * Random_Wiener;
                                
                                At_Update = Astar + Var * Yt;
                                
                                //_______________________________Added by Samer Hamdar: 13 October 2008
                                
                                At_freeflow = ( Amax_D * ( 1 - ( Vt_Update / Vo_D ) ) );
                                
                                //Compute Variables for Hazard Computation
                                
                                AverageSurroundingDensitytmp = 0.0;
                                AverageSurroundingDensityLaneRtmp = 0.0;
                                AverageSurroundingDensityLaneLtmp = 0.0;
                                
                                x = 0;//to track frametime data
                                y = 0;//to track frametime.framelocation data
                                
                                while (x < t) //Search for the Frame Limiting The Episode in the Sorted Data in Time Level
                                {
                                    y = 0;// Initialize y to go through FrameLocations Corresponding to Frame Time Slot x
                                    
                                    while (y < FrameTime[x].NumFramesLocation)//Search for the Frame Limiting The Episode in the Sorted Data in Space Level
                                    {
                                        if (FrameTime[x].FrameLocation[y].FrameGlobalID == Vehicles[n].VehFrames[f].FrameGlobalID)
                                        {
                                            x_c = x;
                                            y_c = y;
                                            
                                            //Speed
                                            Vehicles[n].VehFrames[f].DriverSpeed_Frame = Vt_Update;
                                            
                                            //Distance_To_Ramp
                                            Vehicles[n].VehFrames[f].DistanceToRamp_Frame = fabs ( RampLength - Xt_Update);
                                            
                                            //First Leader Related Data (Same Lane)
                                            sw = FrameTime[x].NumFramesLocation - y;
                                            
                                            dummy = 0;
                                            for (int w = 1; w < sw; w++)
                                            {
                                                if (FrameTime[x].FrameLocation[y].Lane == FrameTime[x].FrameLocation[y+w].Lane)
                                                {
                                                    dummy = 1;
                                                    Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame = (Vt_Update - FrameTime[x].FrameLocation[y+w].VehicleVelocity);
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayLeader1_Frame = (FrameTime[x].FrameLocation[y+w].LocalY - Xt_Update);
                                                    w = sw+1;
                                                }
                                            }
                                            if (dummy == 0)//No First Leader
                                            {
                                                if ( FrameTime[x].FrameLocation[y].Lane == 7 )
                                                {
                                                    Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame = Vt_Update;
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayLeader1_Frame = ( RampLength - Xt_Update);
                                                }
                                                else
                                                {
                                                    Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame = -1.1234;
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayLeader1_Frame = 20.1234;
                                                    
                                                    Vehicles[n].VehFrames[f].Flag1 = 1;
                                                }
                                            }
                                            
                                            //Second Leader Related Data (Same Lane)
                                            dummy = 0;
                                            track = 2;
                                            for (int w = 1; w < sw; w++)
                                            {
                                                if (FrameTime[x].FrameLocation[y].Lane == FrameTime[x].FrameLocation[y+w].Lane)
                                                {
                                                    track --;
                                                    if (track == 0)
                                                    {
                                                        dummy = 1;
                                                        Vehicles[n].VehFrames[f].RelativeSpeedLeader2_Frame = ( Vt_Update - FrameTime[x].FrameLocation[y+w].VehicleVelocity);
                                                        Vehicles[n].VehFrames[f].DistanceHeadwayLeader2_Frame = (FrameTime[x].FrameLocation[y+w].LocalY - Xt_Update);
                                                        w = sw+1;
                                                    }
                                                }
                                            }
                                            if (dummy == 0)//No Second Leader
                                            {
                                                Vehicles[n].VehFrames[f].RelativeSpeedLeader2_Frame = -1.1234;
                                                Vehicles[n].VehFrames[f].DistanceHeadwayLeader2_Frame = 35.1234;
                                                
                                                Vehicles[n].VehFrames[f].Flag1 = 1;
                                            }
                                            
                                            //First Follower Related Data (Same Lane)
                                            dummy = 0;
                                            for (int w = 1; w <= y; w++)
                                            {
                                                if (FrameTime[x].FrameLocation[y].Lane == FrameTime[x].FrameLocation[y-w].Lane)
                                                {
                                                    dummy = 1;
                                                    Vehicles[n].VehFrames[f].RelativeSpeedFollower1_Frame = (Vt_Update - FrameTime[x].FrameLocation[y-w].VehicleVelocity);
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayFollower1_Frame = (Xt_Update - FrameTime[x].FrameLocation[y-w].LocalY);
                                                    w = y+1;
                                                }
                                            }
                                            if (dummy == 0)//No First Follower
                                            {
                                                Vehicles[n].VehFrames[f].RelativeSpeedFollower1_Frame = 1.1234;
                                                Vehicles[n].VehFrames[f].DistanceHeadwayFollower1_Frame = 20.1234;
                                                
                                                Vehicles[n].VehFrames[f].Flag1 = 1;
                                            }
                                            
                                            //First Leader Related Data (Left Lane)
                                            sw = FrameTime[x].NumFramesLocation - y;
                                            dummy = 0;
                                            for (int w = 1; w < sw; w++)
                                            {
                                                if (FrameTime[x].FrameLocation[y].Lane == (FrameTime[x].FrameLocation[y+w].Lane + 1))
                                                {
                                                    dummy = 1;
                                                    Vehicles[n].VehFrames[f].RelativeSpeedLeftL1_Frame = ( Vt_Update - FrameTime[x].FrameLocation[y+w].VehicleVelocity);
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayLeftL1_Frame = (FrameTime[x].FrameLocation[y+w].LocalY - Xt_Update);
                                                    w = sw+1;
                                                }
                                            }
                                            if (dummy == 0)//No First Leader / (Lane 1) (NB: Data related to Lane 9 - shoulder are ignored) + (Lane Seven Always Consider Headway Ahead)
                                            {
                                                //veh_duration_data_SUB.RelativeSpeedLeftL1 = 999;
                                                //veh_duration_data_SUB.DistanceHeadwayLeftL1 = 9999;
                                                if ( FrameTime[x].FrameLocation[y].Lane == 1 )
                                                {
                                                    Vehicles[n].VehFrames[f].RelativeSpeedLeftL1_Frame = 30.1234;
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayLeftL1_Frame = 0.1234;
                                                }
                                                else
                                                {
                                                    Vehicles[n].VehFrames[f].RelativeSpeedLeftL1_Frame = -1.1234;
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayLeftL1_Frame = 20.1234;
                                                    
                                                    Vehicles[n].VehFrames[f].Flag1 = 1;
                                                }
                                            }
                                            
                                            //First Leader Related Data (Right Lane)
                                            dummy = 0;
                                            for (int w = 1; w < sw; w++)
                                            {
                                                if (FrameTime[x].FrameLocation[y].Lane == (FrameTime[x].FrameLocation[y+w].Lane - 1) && FrameTime[x].FrameLocation[y].Lane != 6)
                                                {
                                                    dummy = 1;
                                                    Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame = (Vt_Update - FrameTime[x].FrameLocation[y+w].VehicleVelocity);
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame = (FrameTime[x].FrameLocation[y+w].LocalY - Xt_Update);
                                                    w = sw+1;
                                                }
                                                else if (  FrameTime[x].FrameLocation[y].Lane == 6  )
                                                {
                                                    if ( FrameTime[x].FrameLocation[y].LocalY > 91 && FrameTime[x].FrameLocation[y].LocalY < 215)
                                                    {
                                                        dummy = 1;
                                                        Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame = (Vt_Update - FrameTime[x].FrameLocation[y+w].VehicleVelocity);
                                                        Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame = (FrameTime[x].FrameLocation[y+w].LocalY - Xt_Update);
                                                        w = sw+1;
                                                        
                                                        Vehicles[n].VehFrames[f].Flag2 = 1;
                                                    }
                                                    else
                                                    {
                                                        dummy = 1;
                                                        Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame = 30.1234;
                                                        Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame = 0.1234;
                                                    }
                                                    
                                                }
                                            }
                                            if (dummy == 0)//No First Leader / Lane 7/ (NB: Data related to Lane 9 - shoulder are ignored)
                                            {
                                                //veh_duration_data_SUB.RelativeSpeedRightL1 = 999;
                                                //veh_duration_data_SUB.DistanceHeadwayRightL1 = 9999;
                                                if ( FrameTime[x].FrameLocation[y].Lane == 7 )
                                                {
                                                    Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame = 30.1234;
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame = 0.1234;
                                                }
                                                else
                                                {
                                                    Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame = -1.1234;
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame = 20.1234;
                                                    
                                                    Vehicles[n].VehFrames[f].Flag1 = 1;
                                                }
                                            }
                                            
                                            
                                            //First Follower Related Data (Left Lane)
                                            dummy = 0;
                                            for (int w = 1; w <= y; w++)
                                            {
                                                if (FrameTime[x].FrameLocation[y].Lane == (FrameTime[x].FrameLocation[y-w].Lane + 1))
                                                {
                                                    dummy = 1;
                                                    Vehicles[n].VehFrames[f].RelativeSpeedLeftF1_Frame =(Vt_Update - FrameTime[x].FrameLocation[y-w].VehicleVelocity);
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayLeftF1_Frame = (Xt_Update - FrameTime[x].FrameLocation[y-w].LocalY);
                                                    w = y+1;
                                                }
                                            }
                                            if (dummy == 0)//No First Follower / (Lane 1)
                                            {
                                                //veh_duration_data_SUB.RelativeSpeedLeftF1 = 999;
                                                //veh_duration_data_SUB.DistanceHeadwayLeftF1 = 9999;
                                                if ( FrameTime[x].FrameLocation[y].Lane == 1 )
                                                {
                                                    Vehicles[n].VehFrames[f].RelativeSpeedLeftF1_Frame = -30.1234;
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayLeftF1_Frame = 0.1234;
                                                }
                                                else
                                                {
                                                    Vehicles[n].VehFrames[f].RelativeSpeedLeftF1_Frame = 1.1234;
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayLeftF1_Frame = 20.1234;
                                                    
                                                    Vehicles[n].VehFrames[f].Flag1 = 1;
                                                }
                                            }
                                            
                                            //First Follower Related Data (Right Lane)
                                            dummy = 0;
                                            for (int w = 1; w <= y; w++)
                                            {
                                                if (FrameTime[x].FrameLocation[y].Lane == (FrameTime[x].FrameLocation[y-w].Lane - 1) && FrameTime[x].FrameLocation[y].Lane != 6)
                                                {
                                                    dummy = 1;
                                                    Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame = (Vt_Update - FrameTime[x].FrameLocation[y-w].VehicleVelocity);
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame = (Xt_Update - FrameTime[x].FrameLocation[y-w].LocalY);
                                                    w = y+1;
                                                }
                                                else if ( FrameTime[x].FrameLocation[y].Lane == 6 )
                                                {
                                                    if ( FrameTime[x].FrameLocation[y].LocalY > 91 && FrameTime[x].FrameLocation[y].LocalY < 215)
                                                    {
                                                        dummy = 1;
                                                        Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame = (Vt_Update - FrameTime[x].FrameLocation[y-w].VehicleVelocity);
                                                        Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame = (Xt_Update - FrameTime[x].FrameLocation[y-w].LocalY);
                                                        w = y+1;
                                                        
                                                        Vehicles[n].VehFrames[f].Flag2 = 1;
                                                    }
                                                    else
                                                    {
                                                        dummy = 1;
                                                        Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame = -30.1234;
                                                        Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame = 0.1234;
                                                    }
                                                }
                                            }
                                            if (dummy == 0)//No First Follower / (Lane 7)
                                            {
                                                //veh_duration_data_SUB.RelativeSpeedRightF1 = 999;
                                                //veh_duration_data_SUB.DistanceHeadwayRightF1 = 9999;
                                                if ( FrameTime[x].FrameLocation[y].Lane == 7 )
                                                {
                                                    Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame = - 30.1234;
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame = 0.1234;
                                                }
                                                else
                                                {
                                                    Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame = 1.1234;
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame = 20.1234;
                                                    
                                                    Vehicles[n].VehFrames[f].Flag1 = 1;
                                                }
                                            }
                                            
                                            //Average Density Related Data
                                            AverageSurroundingDensitytmp = 1;//Modified by Samer Hamdar, April 5, 2007: the vehicle of interest is considered in the surrounding density
                                            AverageSurroundingDensityLaneRtmp = 0;
                                            AverageSurroundingDensityLaneLtmp = 0;
                                            
                                            //General
                                            ww = 1;
                                            //Ahead
                                            while ( ww < sw)
                                            {
                                                if ( FrameTime[x].FrameLocation[y + ww].LocalY - Xt_Update <= DistanceDensityAhead)
                                                {
                                                    AverageSurroundingDensitytmp = AverageSurroundingDensitytmp + 1;
                                                }
                                                ww = ww + 1;
                                            }
                                            
                                            //Behind
                                            ww = 1;
                                            while (ww <= y)
                                            {
                                                if (  Xt_Update  - FrameTime[x].FrameLocation[y - ww].LocalY <= DistanceDensityBehind)
                                                {
                                                    AverageSurroundingDensitytmp = AverageSurroundingDensitytmp + 1;
                                                }
                                                ww = ww + 1;
                                            }
                                            
                                            //Interpolation
                                            if ((Xt_Update < DistanceDensityBehind) && (L - Xt_Update >= DistanceDensityAhead))
                                            {
                                                AverageSurroundingDensitytmp = (AverageSurroundingDensitytmp * (DistanceDensityAhead + DistanceDensityBehind) / (Xt_Update + DistanceDensityAhead));
                                            }
                                            
                                            if ((Xt_Update >= DistanceDensityBehind) && (L - Xt_Update < DistanceDensityAhead))
                                            {
                                                AverageSurroundingDensitytmp = (AverageSurroundingDensitytmp * (DistanceDensityAhead + DistanceDensityBehind) / (DistanceDensityBehind + L - Xt_Update));
                                            }
                                            
                                            if ((Xt_Update < DistanceDensityBehind) && (L - Xt_Update < DistanceDensityAhead))
                                            {
                                                AverageSurroundingDensitytmp = (AverageSurroundingDensitytmp * (DistanceDensityAhead + DistanceDensityBehind) / L);
                                            }
                                            
                                            AverageSurroundingDensitytmp = ((AverageSurroundingDensitytmp * 1000.0)/(DistanceDensityAhead + DistanceDensityBehind) / NLanes);
                                            Vehicles[n].VehFrames[f].AverageSurroundingDensity_Frame = AverageSurroundingDensitytmp;
                                            
                                            
                                            //Average Density to the Right Related Data
                                            ww = 1;
                                            //Ahead
                                            while ( ww < sw)
                                            {
                                                if ((FrameTime[x].FrameLocation[y + ww].LocalY - Xt_Update <= DistanceDensityAhead) && (FrameTime[x].FrameLocation[y].Lane == (FrameTime[x].FrameLocation[y + ww].Lane - 1) ) )
                                                {
                                                    AverageSurroundingDensityLaneRtmp = AverageSurroundingDensityLaneRtmp + 1;
                                                }
                                                ww = ww + 1;
                                            }
                                            
                                            //Behind
                                            ww = 1;
                                            while (ww <= y)
                                            {
                                                if (  Xt_Update  - FrameTime[x].FrameLocation[y - ww].LocalY <= DistanceDensityBehind && (FrameTime[x].FrameLocation[y].Lane == (FrameTime[x].FrameLocation[y-ww].Lane - 1)))
                                                {
                                                    AverageSurroundingDensityLaneRtmp = AverageSurroundingDensityLaneRtmp + 1;
                                                }
                                                ww = ww + 1;
                                            }
                                            
                                            //Interpolation
                                            if ((Xt_Update < DistanceDensityBehind) && (L - Xt_Update >= DistanceDensityAhead))
                                            {
                                                AverageSurroundingDensityLaneRtmp = (AverageSurroundingDensityLaneRtmp * (DistanceDensityAhead + DistanceDensityBehind) / (Xt_Update + DistanceDensityAhead));
                                            }
                                            
                                            if ((Xt_Update >= DistanceDensityBehind) && (L - Xt_Update < DistanceDensityAhead))
                                            {
                                                AverageSurroundingDensityLaneRtmp = (AverageSurroundingDensityLaneRtmp * (DistanceDensityAhead + DistanceDensityBehind) / (DistanceDensityBehind + L - Xt_Update));
                                            }
                                            
                                            if ((Xt_Update < DistanceDensityBehind) && (L - Xt_Update < DistanceDensityAhead))
                                            {
                                                AverageSurroundingDensityLaneRtmp = (AverageSurroundingDensityLaneRtmp * (DistanceDensityAhead + DistanceDensityBehind) / L);
                                            }
                                            
                                            AverageSurroundingDensityLaneRtmp = ((AverageSurroundingDensityLaneRtmp * 1000.0)/(DistanceDensityAhead + DistanceDensityBehind));// 1 mile = 5280 fts
                                            
                                            
                                            if (FrameTime[x].FrameLocation[y].Lane == 6)
                                            {
                                                if ( Xt_Update > 91 && Xt_Update < 215)
                                                {
                                                    AverageSurroundingDensityLaneRtmp = AverageSurroundingDensityLaneRtmp;
                                                }
                                                else
                                                {
                                                    AverageSurroundingDensityLaneRtmp = 100.1234;
                                                    
                                                }
                                            }
                                            
                                            if (FrameTime[x].FrameLocation[y].Lane == 7)
                                            {
                                                AverageSurroundingDensityLaneRtmp = 100.1234;
                                            }
                                            
                                            Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneR_Frame = AverageSurroundingDensityLaneRtmp;
                                            
                                            //Average Density to the Left Related Data
                                            ww = 1;
                                            //Ahead
                                            while ( ww < sw)
                                            {
                                                if ((FrameTime[x].FrameLocation[y + ww].LocalY - Xt_Update <= DistanceDensityAhead) && (FrameTime[x].FrameLocation[y].Lane == (FrameTime[x].FrameLocation[y + ww].Lane + 1)) )
                                                {
                                                    AverageSurroundingDensityLaneLtmp = AverageSurroundingDensityLaneLtmp + 1;
                                                }
                                                ww = ww + 1;
                                            }
                                            
                                            //Behind
                                            ww = 1;
                                            while (ww <= y)
                                            {
                                                if (  ( Xt_Update  - FrameTime[x].FrameLocation[y-ww].LocalY <= DistanceDensityBehind ) && ( FrameTime[x].FrameLocation[y].Lane == (FrameTime[x].FrameLocation[y-ww].Lane + 1) ) )
                                                {
                                                    AverageSurroundingDensityLaneLtmp = AverageSurroundingDensityLaneLtmp + 1;
                                                }
                                                ww = ww + 1;
                                            }
                                            
                                            //Interpolation
                                            if ((Xt_Update < DistanceDensityBehind) && (L - Xt_Update >= DistanceDensityAhead))
                                            {
                                                AverageSurroundingDensityLaneLtmp = (AverageSurroundingDensityLaneLtmp * (DistanceDensityAhead + DistanceDensityBehind) / (Xt_Update + DistanceDensityAhead));
                                            }
                                            
                                            if ((Xt_Update >= DistanceDensityBehind) && (L - Xt_Update < DistanceDensityAhead))
                                            {
                                                AverageSurroundingDensityLaneLtmp = (AverageSurroundingDensityLaneLtmp * (DistanceDensityAhead + DistanceDensityBehind) / (DistanceDensityBehind + L - Xt_Update));
                                            }
                                            
                                            if ((Xt_Update < DistanceDensityBehind) && (L - Xt_Update < DistanceDensityAhead))
                                            {
                                                AverageSurroundingDensityLaneLtmp = (AverageSurroundingDensityLaneLtmp * (DistanceDensityAhead + DistanceDensityBehind) / L);
                                                
                                            }
                                            
                                            AverageSurroundingDensityLaneLtmp = ((AverageSurroundingDensityLaneLtmp * 1000.0)/(DistanceDensityAhead + DistanceDensityBehind));// 1 mile = 5280 fts
                                            
                                            if (FrameTime[x].FrameLocation[y].Lane == 1)
                                            {
                                                AverageSurroundingDensityLaneLtmp = 100.1234;
                                            }
                                            
                                            Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneL_Frame = AverageSurroundingDensityLaneLtmp;
                                            
                                            break;
                                        }
                                        y++;
                                    }
                                    x++;
                                }
                                
                                //Hazard: Car-Following wih Lane Change Exit
                                
                                //__Assuming Weibull Distribution
                                
                                //____Number One: Limdep Technique
                                Lambda_tmp = exp ( -1.0 * ( BetaOne_CF_LC + BetaLCL_CF_LC * Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame + BetaV_CF_LC * Vehicles[n].VehFrames[f].DriverSpeed_Frame + BetaDXL1_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeader1_Frame + BetaDVL1_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame + BetaDXL2_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeader2_Frame + BetaDVL2_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeader2_Frame + BetaDXF1_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayFollower1_Frame + BetaDVF1_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedFollower1_Frame + BetaDXL1R_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame + BetaDVL1R_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame + BetaDXF1R_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame + BetaDVF1R_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame + BetaDXL1L_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeftL1_Frame + BetaDVL1L_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeftL1_Frame + BetaDXF1L_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeftF1_Frame + BetaDVF1L_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeftF1_Frame + BetaKR_CF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneR_Frame + BetaKL_CF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneL_Frame + BetaK_CF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensity_Frame + BetaRightLane_CF_LC * Vehicles[n].VehFrames[f].RightLaneDummy_Frame + BetaLeftLane_CF_LC * Vehicles[n].VehFrames[f].LeftLaneDummy_Frame + BetaRamp_CF_LC * Vehicles[n].VehFrames[f].RampDummy_Frame + BetaDistanceToRamp_CF_LC * Vehicles[n].VehFrames[f].DistanceToRamp_Frame) );
                                
                                Hazard_ParameterA_CF_LC = Lambda_tmp;
                                Hazard_tmp = Hazard_ParameterA_CF_LC * Hazard_ParameterB_CF_LC * pow ( ( Hazard_ParameterA_CF_LC * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ),( Hazard_ParameterB_CF_LC - 1 ) );
                                Survival_tmp = exp (  -1.0 * pow ( ( Hazard_ParameterA_CF_LC * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ) , Hazard_ParameterB_CF_LC )  );
                                
                                R_Heterogeneity_CF_LC = K_Heterogeneity_CF_LC;
                                
                                //Teta_tmp = ( 1.0 / K_Heterogeneity_CF_LC );
                                Teta_tmp = 0.55243252;
                                
                                Survival2_tmp = Survival_tmp * Teta_tmp;
                                
                                Hazard2_tmp = Survival2_tmp * Hazard_tmp;
                                
                                //____Number Two: Bhat Technique
                                //R_Heterogeneity_CF_LC = K_Heterogeneity_CF_LC;
                                //Hazard_tmp = Hazard_ParameterA_CF_LC * Hazard_ParameterB_CF_LC * pow ( ( Hazard_ParameterA_CF_LC * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ),( Hazard_ParameterB_CF_LC - 1 ) );
                                //Survival_tmp = exp (  -1.0 * pow ( ( Hazard_ParameterA_CF_LC * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ) , Hazard_ParameterB_CF_LC )  );
                                //
                                ////Wrong:Gamma_tmp = ( pow ( K_Heterogeneity_CF_LC, R_Heterogeneity_CF_LC ) / double ( factorial ( int ( R_Heterogeneity_CF_LC - 1 ) ) ) ) * exp ( - 1.0 * K_Heterogeneity_CF_LC * V_tmp) * pow ( V_tmp , ( R_Heterogeneity_CF_LC - 1)  );
                                //
                                //Gamma_tmp = Vehicles[n].VehFrames[f].GammaHeterogeneity_CF_LC;
                                //W_tmp = log (Gamma_tmp);//(?): What is
                                //Lambda_tmp = exp ( -1.0 * ( BetaOne_CF_LC + BetaLCL_CF_LC * Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame + BetaV_CF_LC * Vehicles[n].VehFrames[f].DriverSpeed_Frame + BetaDXL1_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeader1_Frame + BetaDVL1_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame + BetaDXL2_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeader2_Frame + BetaDVL2_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeader2_Frame + BetaDXF1_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayFollower1_Frame + BetaDVF1_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedFollower1_Frame + BetaDXL1R_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame + BetaDVL1R_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame + BetaDXF1R_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame + BetaDVF1R_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame + BetaDXL1L_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeftL1_Frame + BetaDVL1L_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeftL1_Frame + BetaDXF1L_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeftF1_Frame + BetaDVF1L_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeftF1_Frame + BetaKR_CF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneR_Frame + BetaKL_CF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneL_Frame + BetaK_CF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensity_Frame + BetaRightLane_CF_LC * Vehicles[n].VehFrames[f].RightLaneDummy_Frame + BetaLeftLane_CF_LC * Vehicles[n].VehFrames[f].LeftLaneDummy_Frame + BetaRamp_CF_LC * Vehicles[n].VehFrames[f].RampDummy_Frame + BetaDistanceToRamp_CF_LC * Vehicles[n].VehFrames[f].DistanceToRamp_Frame)+ W_tmp );
                                //Hazard2_tmp = Hazard_tmp * Lambda_tmp;
                                
                                Vehicles[n].VehFrames[f].LaneChange_Hazard_CF = Hazard2_tmp;
                                
                                //Hazard: Car-Following with Mode Exit
                                
                                //__Assuming Weibull Distribution
                                
                                //____Number One: Limdep Technique
                                Lambda_tmp = exp ( -1.0 * ( BetaOne_CF_CM + BetaLCL_CF_CM * Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame + BetaV_CF_CM * Vehicles[n].VehFrames[f].DriverSpeed_Frame + BetaDXL1_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeader1_Frame + BetaDVL1_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame + BetaDXL2_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeader2_Frame + BetaDVL2_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeader2_Frame + BetaDXF1_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayFollower1_Frame + BetaDVF1_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedFollower1_Frame + BetaDXL1R_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame + BetaDVL1R_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame + BetaDXF1R_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame + BetaDVF1R_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame + BetaDXL1L_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeftL1_Frame + BetaDVL1L_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeftL1_Frame + BetaDXF1L_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeftF1_Frame + BetaDVF1L_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeftF1_Frame + BetaKR_CF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneR_Frame + BetaKL_CF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneL_Frame + BetaK_CF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensity_Frame + BetaRightLane_CF_CM * Vehicles[n].VehFrames[f].RightLaneDummy_Frame + BetaLeftLane_CF_CM * Vehicles[n].VehFrames[f].LeftLaneDummy_Frame + BetaRamp_CF_CM * Vehicles[n].VehFrames[f].RampDummy_Frame + BetaDistanceToRamp_CF_CM * Vehicles[n].VehFrames[f].DistanceToRamp_Frame) );
                                
                                Hazard_ParameterA_CF_CM = Lambda_tmp;
                                Hazard_tmp = Hazard_ParameterA_CF_CM * Hazard_ParameterB_CF_CM * pow ( ( Hazard_ParameterA_CF_CM * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ),( Hazard_ParameterB_CF_CM - 1 ) );
                                Survival_tmp = exp (  -1.0 * pow ( ( Hazard_ParameterA_CF_CM * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ) , Hazard_ParameterB_CF_CM )  );
                                
                                R_Heterogeneity_CF_CM = K_Heterogeneity_CF_CM;
                                
                                //Teta_tmp = ( 1.0 / K_Heterogeneity_CF_CM );
                                Teta_tmp = 0.55243252;
                                
                                Survival2_tmp = Survival_tmp * Teta_tmp;
                                
                                Hazard2_tmp = Survival2_tmp * Hazard_tmp;
                                
                                //____Number Two: Bhat Technique
                                //R_Heterogeneity_CF_CM = K_Heterogeneity_CF_CM;
                                //Hazard_tmp = Hazard_ParameterA_CF_CM * Hazard_ParameterB_CF_CM * pow ( ( Hazard_ParameterA_CF_CM * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ),( Hazard_ParameterB_CF_CM - 1 ) );
                                //Survival_tmp = exp (  -1.0 * pow ( ( Hazard_ParameterA_CF_CM * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ) , Hazard_ParameterB_CF_CM )  );
                                //
                                ////Wrong:Gamma_tmp = ( pow ( K_Heterogeneity_CF_CM, R_Heterogeneity_CF_CM ) / double ( factorial ( int ( R_Heterogeneity_CF_CM - 1 ) ) ) ) * exp ( - 1.0 * K_Heterogeneity_CF_CM * V_tmp) * pow ( V_tmp , ( R_Heterogeneity_CF_CM - 1)  );
                                //
                                //Gamma_tmp = Vehicles[n].VehFrames[f].GammaHeterogeneity_CF_CM;
                                //W_tmp = log (Gamma_tmp);//(?): What is
                                //Lambda_tmp = exp ( -1.0 * ( BetaOne_CF_CM + BetaLCL_CF_CM * Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame + BetaV_CF_CM * Vehicles[n].VehFrames[f].DriverSpeed_Frame + BetaDXL1_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeader1_Frame + BetaDVL1_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame + BetaDXL2_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeader2_Frame + BetaDVL2_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeader2_Frame + BetaDXF1_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayFollower1_Frame + BetaDVF1_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedFollower1_Frame + BetaDXL1R_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame + BetaDVL1R_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame + BetaDXF1R_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame + BetaDVF1R_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame + BetaDXL1L_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeftL1_Frame + BetaDVL1L_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeftL1_Frame + BetaDXF1L_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeftF1_Frame + BetaDVF1L_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeftF1_Frame + BetaKR_CF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneR_Frame + BetaKL_CF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneL_Frame + BetaK_CF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensity_Frame + BetaRightLane_CF_CM * Vehicles[n].VehFrames[f].RightLaneDummy_Frame + BetaLeftLane_CF_CM * Vehicles[n].VehFrames[f].LeftLaneDummy_Frame + BetaRamp_CF_CM * Vehicles[n].VehFrames[f].RampDummy_Frame + BetaDistanceToRamp_CF_CM * Vehicles[n].VehFrames[f].DistanceToRamp_Frame)+ W_tmp );
                                //Hazard2_tmp = Hazard_tmp * Lambda_tmp;
                                
                                Vehicles[n].VehFrames[f].Mode_Hazard_CF = Hazard2_tmp;
                                
                                //Hazard: Free Flow with Lane Change Exit
                                
                                //__Assuming Exponential Distribution
                                
                                //____Number One: Limdep Technique
                                Lambda_tmp = exp ( -1.0 * ( BetaOne_FF_LC + BetaLCL_FF_LC * Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame + BetaV_FF_LC * Vehicles[n].VehFrames[f].DriverSpeed_Frame + BetaDXL1_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeader1_Frame + BetaDVL1_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame + BetaDXL2_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeader2_Frame + BetaDVL2_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeader2_Frame + BetaDXF1_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayFollower1_Frame + BetaDVF1_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedFollower1_Frame + BetaDXL1R_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame + BetaDVL1R_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame + BetaDXF1R_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame + BetaDVF1R_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame + BetaDXL1L_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeftL1_Frame + BetaDVL1L_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeftL1_Frame + BetaDXF1L_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeftF1_Frame + BetaDVF1L_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeftF1_Frame + BetaKR_FF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneR_Frame + BetaKL_FF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneL_Frame + BetaK_FF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensity_Frame + BetaRightLane_FF_LC * Vehicles[n].VehFrames[f].RightLaneDummy_Frame + BetaLeftLane_FF_LC * Vehicles[n].VehFrames[f].LeftLaneDummy_Frame + BetaRamp_FF_LC * Vehicles[n].VehFrames[f].RampDummy_Frame + BetaDistanceToRamp_FF_LC * Vehicles[n].VehFrames[f].DistanceToRamp_Frame) );
                                
                                Hazard_ParameterA_FF_LC = Lambda_tmp;
                                Hazard_ParameterB_FF_LC = 1.0;//Difference Between Exponential and Weibull
                                Hazard_tmp = Hazard_ParameterA_FF_LC * Hazard_ParameterB_FF_LC * pow ( ( Hazard_ParameterA_FF_LC * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ),( Hazard_ParameterB_FF_LC - 1 ) );
                                Survival_tmp = exp (  -1.0 * pow ( ( Hazard_ParameterA_FF_LC * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ) , Hazard_ParameterB_FF_LC )  );
                                
                                R_Heterogeneity_FF_LC = K_Heterogeneity_FF_LC;
                                
                                //Teta_tmp = ( 1.0 / K_Heterogeneity_FF_LC );
                                Teta_tmp = 0.55243252;
                                
                                Survival2_tmp = Survival_tmp * Teta_tmp;
                                
                                Hazard2_tmp = Survival2_tmp * Hazard_tmp;
                                
                                //____Number Two: Bhat Technique
                                //R_Heterogeneity_FF_LC = K_Heterogeneity_FF_LC;
                                //Hazard_ParameterB_FF_LC = 1.0;
                                //Hazard_tmp = Hazard_ParameterA_FF_LC * Hazard_ParameterB_FF_LC * pow ( ( Hazard_ParameterA_FF_LC * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ),( Hazard_ParameterB_FF_LC - 1 ) );
                                //Survival_tmp = exp (  -1.0 * pow ( ( Hazard_ParameterA_FF_LC * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ) , Hazard_ParameterB_FF_LC )  );
                                //
                                ////Wrong:Gamma_tmp = ( pow ( K_Heterogeneity_FF_LC, R_Heterogeneity_FF_LC ) / double ( factorial ( int ( R_Heterogeneity_FF_LC - 1 ) ) ) ) * exp ( - 1.0 * K_Heterogeneity_FF_LC * V_tmp) * pow ( V_tmp , ( R_Heterogeneity_FF_LC - 1)  );
                                //
                                ////Gamma_tmp = Vehicles[n].VehFrames[f].GammaHeterogeneity_FF_LC;
                                //W_tmp = log (Gamma_tmp);//(?): What is v
                                //Lambda_tmp = exp ( -1.0 * ( BetaOne_FF_LC + BetaLCL_FF_LC * Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame + BetaV_FF_LC * Vehicles[n].VehFrames[f].DriverSpeed_Frame + BetaDXL1_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeader1_Frame + BetaDVL1_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame + BetaDXL2_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeader2_Frame + BetaDVL2_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeader2_Frame + BetaDXF1_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayFollower1_Frame + BetaDVF1_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedFollower1_Frame + BetaDXL1R_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame + BetaDVL1R_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame + BetaDXF1R_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame + BetaDVF1R_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame + BetaDXL1L_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeftL1_Frame + BetaDVL1L_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeftL1_Frame + BetaDXF1L_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeftF1_Frame + BetaDVF1L_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeftF1_Frame + BetaKR_FF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneR_Frame + BetaKL_FF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneL_Frame + BetaK_FF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensity_Frame + BetaRightLane_FF_LC * Vehicles[n].VehFrames[f].RightLaneDummy_Frame + BetaLeftLane_FF_LC * Vehicles[n].VehFrames[f].LeftLaneDummy_Frame + BetaRamp_FF_LC * Vehicles[n].VehFrames[f].RampDummy_Frame + BetaDistanceToRamp_FF_LC * Vehicles[n].VehFrames[f].DistanceToRamp_Frame)+ W_tmp );
                                //Hazard2_tmp = Hazard_tmp * Lambda_tmp;
                                
                                Vehicles[n].VehFrames[f].LaneChange_Hazard_FF = Hazard2_tmp;
                                
                                //Hazard: Free-Flow with Mode Exit
                                //__Assuming Exponential Distribution
                                
                                //____Number One: Limdep Technique
                                Lambda_tmp = exp ( -1.0 * ( BetaOne_FF_CM + BetaLCL_FF_CM * Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame + BetaV_FF_CM * Vehicles[n].VehFrames[f].DriverSpeed_Frame + BetaDXL1_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeader1_Frame + BetaDVL1_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame + BetaDXL2_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeader2_Frame + BetaDVL2_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeader2_Frame + BetaDXF1_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayFollower1_Frame + BetaDVF1_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedFollower1_Frame + BetaDXL1R_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame + BetaDVL1R_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame + BetaDXF1R_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame + BetaDVF1R_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame + BetaDXL1L_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeftL1_Frame + BetaDVL1L_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeftL1_Frame + BetaDXF1L_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeftF1_Frame + BetaDVF1L_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeftF1_Frame + BetaKR_FF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneR_Frame + BetaKL_FF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneL_Frame + BetaK_FF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensity_Frame + BetaRightLane_FF_CM * Vehicles[n].VehFrames[f].RightLaneDummy_Frame + BetaLeftLane_FF_CM * Vehicles[n].VehFrames[f].LeftLaneDummy_Frame + BetaRamp_FF_CM * Vehicles[n].VehFrames[f].RampDummy_Frame + BetaDistanceToRamp_FF_CM * Vehicles[n].VehFrames[f].DistanceToRamp_Frame) );
                                
                                Hazard_ParameterA_FF_CM = Lambda_tmp;
                                Hazard_ParameterB_FF_CM = 1.0;//Difference Between Exponential and Weibull
                                Hazard_tmp = Hazard_ParameterA_FF_CM * Hazard_ParameterB_FF_CM * pow ( ( Hazard_ParameterA_FF_CM * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ),( Hazard_ParameterB_FF_CM - 1 ) );
                                Survival_tmp = exp (  -1.0 * pow ( ( Hazard_ParameterA_FF_CM * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ) , Hazard_ParameterB_FF_CM )  );
                                
                                R_Heterogeneity_FF_CM = K_Heterogeneity_FF_CM;
                                
                                //Teta_tmp = ( 1.0 / K_Heterogeneity_FF_CM );
                                Teta_tmp = 0.55243252;
                                
                                Survival2_tmp = Survival_tmp * Teta_tmp;
                                
                                Hazard2_tmp = Survival2_tmp * Hazard_tmp;
                                
                                //____Number Two: Bhat Technique
                                //R_Heterogeneity_FF_CM = K_Heterogeneity_FF_CM;
                                //Hazard_ParameterB_FF_CM = 1.0;
                                //Hazard_tmp = Hazard_ParameterA_FF_CM * Hazard_ParameterB_FF_CM * pow ( ( Hazard_ParameterA_FF_CM * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ),( Hazard_ParameterB_FF_CM - 1 ) );
                                //Survival_tmp = exp (  -1.0 * pow ( ( Hazard_ParameterA_FF_CM * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ) , Hazard_ParameterB_FF_CM )  );
                                //
                                ////Wrong:Gamma_tmp = ( pow ( K_Heterogeneity_FF_CM, R_Heterogeneity_FF_CM ) / double ( factorial ( int ( R_Heterogeneity_FF_CM - 1 ) ) ) ) * exp ( - 1.0 * K_Heterogeneity_FF_CM * V_tmp) * pow ( V_tmp , ( R_Heterogeneity_FF_CM - 1)  );
                                //
                                ////Gamma_tmp = Vehicles[n].VehFrames[f].GammaHeterogeneity_FF_CM;
                                //W_tmp = log (Gamma_tmp);//(?): What is v
                                //Lambda_tmp = exp ( -1.0 * ( BetaOne_FF_CM + BetaLCL_FF_CM * Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame + BetaV_FF_CM * Vehicles[n].VehFrames[f].DriverSpeed_Frame + BetaDXL1_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeader1_Frame + BetaDVL1_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame + BetaDXL2_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeader2_Frame + BetaDVL2_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeader2_Frame + BetaDXF1_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayFollower1_Frame + BetaDVF1_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedFollower1_Frame + BetaDXL1R_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame + BetaDVL1R_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame + BetaDXF1R_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame + BetaDVF1R_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame + BetaDXL1L_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeftL1_Frame + BetaDVL1L_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeftL1_Frame + BetaDXF1L_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeftF1_Frame + BetaDVF1L_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeftF1_Frame + BetaKR_FF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneR_Frame + BetaKL_FF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneL_Frame + BetaK_FF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensity_Frame + BetaRightLane_FF_CM * Vehicles[n].VehFrames[f].RightLaneDummy_Frame + BetaLeftLane_FF_CM * Vehicles[n].VehFrames[f].LeftLaneDummy_Frame + BetaRamp_FF_CM * Vehicles[n].VehFrames[f].RampDummy_Frame + BetaDistanceToRamp_FF_CM * Vehicles[n].VehFrames[f].DistanceToRamp_Frame)+ W_tmp );
                                //Hazard2_tmp = Hazard_tmp * Lambda_tmp;
                                
                                Vehicles[n].VehFrames[f].Mode_Hazard_FF = Hazard2_tmp;
                                
                                //Mode Choice: Duration Delimitation
                                Mode_Percentage = ( (rand ()%1000) / 1000.0 );
                                
                                //Test: No Effect if Missing Data
                                if (Vehicles[n].VehFrames[f].Flag1 == 1 || Vehicles[n].VehFrames[f].Flag2 == 1)
                                {
                                    Vehicles[n].VehFrames[f].LaneChange_Hazard_CF = 0.0;
                                    Vehicles[n].VehFrames[f].LaneChange_Hazard_FF = 0.0;
                                    Vehicles[n].VehFrames[f].Mode_Hazard_CF = 0.0;
                                    Vehicles[n].VehFrames[f].Mode_Hazard_FF = 0.0;
                                }
                                
                                {
                                    if (f == 0)
                                    {
                                        //Considered as CF Episode: 1st Episode
                                        if (Mode_Percentage < Vehicles[n].VehFrames[f].Mode_Hazard_CF)//New Episode
                                        {
                                            RT_Counter = 0;
                                            
                                            Vehicles[n].VehFrames[f].Mode = 0;
                                            Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame = 0;
                                            At_Update = At_freeflow;
                                        }
                                        else
                                        {
                                            Vehicles[n].VehFrames[f].Mode = 1;
                                            At_Update = At_Update;
                                        }
                                        
                                    }
                                    else if (Vehicles[n].VehFrames[f].Mode_Previous == 1)
                                    {
                                        //CF Episode
                                        if (Mode_Percentage < Vehicles[n].VehFrames[f].Mode_Hazard_CF)//New Episode
                                        {
                                            RT_Counter = 0;
                                            
                                            Vehicles[n].VehFrames[f].Mode = 0;
                                            Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame = 0;
                                            At_Update = At_freeflow;
                                        }
                                        else
                                        {
                                            Vehicles[n].VehFrames[f].Mode = 1;
                                            At_Update = At_Update;
                                        }
                                    }
                                    else if (Vehicles[n].VehFrames[f].Mode_Previous == 0)
                                    {
                                        //FF Episode
                                        if (Mode_Percentage < Vehicles[n].VehFrames[f].Mode_Hazard_FF)//New Episode
                                        {
                                            RT_Counter = 0;
                                            
                                            Vehicles[n].VehFrames[f].Mode = 1;
                                            Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame = 0;
                                            At_Update = At_Update;
                                        }
                                        else
                                        {
                                            Vehicles[n].VehFrames[f].Mode = 0;
                                            At_Update = At_freeflow;
                                        }
                                    }
                                }
                                
                                //____________________________End of Added by Samer Hamdar: 8 October 2008
                                
                                //Test
                                //Incorporate Free-Speed Contrained Acceleration and Maximum Deceleration (Maximum Acceleration Just in Case
                                //if ( At_Update >  ( Amax_D * ( 1 - ( Vt_Update / Vo_D ) ) ) )
                                //    At_Update =  ( Amax_D * ( 1 - ( Vt_Update / Vo_D ) ) );
                                
                                //if ( At_Update > 4.0 )
                                //    At_Update = 4.0;
                                //if ( At_Update < -6.0 )  //Problem: Test: Put lower limit on acceleration (?)
                                //    At_Update = -6.0;
                                
                                
                                //Find the Resulting Velocity, Relative Velocity, Location, Relative Headway for Updating the Next Acceleration
                                Vt_Update = Vt_Update + ( At_Update * DeltaT );
                                
                                if ( Vt_Update <= 0.0 )//Problem: Test: To Avoid Negative Velocities (?)
                                    Vt_Update = 0.1;
                                
                                Xt_Update = Xt_Update + ( Vt_Update * DeltaT ) + 0.5 * ( At_Update * DeltaT * DeltaT );
                                DeltaX_Update = Vehicles[n].VehFrames[f].SpaceHeadway + ( Vehicles[n].VehFrames[f].LocalY - Xt_Update );
                                DeltaV_Update = Vehicles[n].VehFrames[f].RelativeVelocity + ( Vt_Update - Vehicles[n].VehFrames[f].VehicleVelocity );
                                S_Update = Vehicles[n].VehFrames[f].SpaceGap + ( Vehicles[n].VehFrames[f].LocalY - Xt_Update );//Possible Error: Problem Since Space Gaps are Sometimes Negative: Use Space Headway Instead
                                
                                //To Detect Accidents
                                if (S_Update <= 0)
                                {
                                    S_Update = 0.1;
                                    Vt_Update = 0.1;
                                    At_Update = 0.0;
                                    Vehicles[n].VehParameterChildren_Mixed_S[i].Accident = 1;
                                    
                                }
                                
                                //Store in Wallsten Measures for Each Frame
                                Vehicles[n].VehFrames[f].WallstenAcceleration = At_Update;
                                Vehicles[n].VehFrames[f].WallstenVelocity = Vt_Update;
                                Vehicles[n].VehFrames[f].WallstenX = Xt_Update;
                                Vehicles[n].VehFrames[f].WallstenRelativeSpeed = DeltaV_Update;
                                Vehicles[n].VehFrames[f].WallstenSpaceHeadway = DeltaX_Update;
                                Vehicles[n].VehFrames[f].WallstenGap = S_Update;//Possible Error: Problem Since Space Gaps are Sometimes Negative: Use Space Headway Instead
                                
                                FrameTime[x_c].FrameLocation[y_c].WallstenVelocity = Vehicles[n].VehFrames[f].WallstenVelocity;
                                FrameTime[x_c].FrameLocation[y_c].WallstenX = Vehicles[n].VehFrames[f].WallstenX;
                                FrameTime[x_c].FrameLocation[y_c].LeadersChangingLanes_Frame = Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame;
                                
                            }
                            else if ( ( RT_Counter % RT_Update ) == 0 )
                            {
                                //Testing Parameters for Watch
                                DummyA = Vehicles[n].VehFrames[f].VehicleAcceleration;
                                DummyV = Vehicles[n].VehFrames[f].VehicleVelocity;
                                DummyX = Vehicles[n].VehFrames[f].LocalY;
                                DummyDX = Vehicles[n].VehFrames[f].SpaceHeadway;
                                DummyDV = Vehicles[n].VehFrames[f].RelativeVelocity;
                                DummyDS = Vehicles[n].VehFrames[f].SpaceGap;
                                
                                if (Vehicles[n].VehFrames[f].PreVehID != Leader_Previous)
                                {
                                    //Removed by Samer Hamdar: 8 October 2008
                                    //At_Update = Vehicles[n].VehFrames[f].VehicleAcceleration;
                                    //Vt_Update = Vehicles[n].VehFrames[f].VehicleVelocity;
                                    //Xt_Update = Vehicles[n].VehFrames[f].LocalY;
                                    //DeltaX_Update = Vehicles[n].VehFrames[f].SpaceHeadway;
                                    //DeltaV_Update = Vehicles[n].VehFrames[f].RelativeVelocity;
                                    //S_Update = Vehicles[n].VehFrames[f].SpaceGap;
                                    //End of Removed by Samer Hamdar: 8 October 2008
                                    
                                    RT_Counter = 0;
                                    
                                    //Added by Samer Hamdar: 8 October 2008
                                    
                                    //Changed Lanes
                                    if (Vehicles[n].VehFrames[f].Lane != Lane_Previous)
                                    {
                                        At_Update = Vehicles[n].VehFrames[f].VehicleAcceleration;
                                        Vt_Update = Vehicles[n].VehFrames[f].VehicleVelocity;
                                        Xt_Update = Vehicles[n].VehFrames[f].LocalY;
                                        DeltaX_Update = Vehicles[n].VehFrames[f].SpaceHeadway;
                                        DeltaV_Update = Vehicles[n].VehFrames[f].RelativeVelocity;
                                        S_Update = Vehicles[n].VehFrames[f].SpaceGap;
                                        
                                        //New Episode:
                                        if (f != 0)//Not First Reading
                                        {
                                            Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame = 0;
                                            Vehicles[n].VehFrames[f].FirstEpisodeDummy_Frame = 0;
                                            Vehicles[n].VehFrames[f].DurationTime_Frame = 0;
                                            
                                            
                                            //Store Lane Changing Hazard Struct
                                            Vehicles[n].NumberOfLaneChanges = Vehicles[n].NumberOfLaneChanges + 1;
                                            LaneChangingData lane_changing_data;
                                            lane_changing_data.LaneChangingTime = Vehicles[n].VehFrames[f].FrameID;
                                            
                                            if (Vehicles[n].VehFrames[f].Mode == 1 )
                                            {
                                                lane_changing_data.LaneChangingHazard = Vehicles[n].VehFrames[f - 1].LaneChange_Hazard_CF;
                                                lane_changing_data.CompetingHazard = Vehicles[n].VehFrames[f - 1].Mode_Hazard_CF;
                                                lane_changing_data.Mode = Vehicles[n].VehFrames[f].Mode;
                                            }
                                            else if (Vehicles[n].VehFrames[f].Mode == 0)
                                            {
                                                lane_changing_data.LaneChangingHazard = Vehicles[n].VehFrames[f - 1].LaneChange_Hazard_FF;
                                                lane_changing_data.CompetingHazard = Vehicles[n].VehFrames[f - 1].Mode_Hazard_FF;
                                                lane_changing_data.Mode = Vehicles[n].VehFrames[f].Mode;
                                            }
                                            
                                            Vehicles[n].LaneChanging.push_back(lane_changing_data);
                                        }
                                    }
                                    else //Did not Change Lanes
                                    {
                                        At_Update = Vehicles[n].VehFrames[f-1].WallstenAcceleration;//Since at this step, no Wallsten Acceleration has been created --> taken from previous time step
                                        Vt_Update = Vehicles[n].VehFrames[f-1].WallstenVelocity;
                                        Xt_Update = Vehicles[n].VehFrames[f-1].WallstenX;
                                        
                                        //NB: DOUBT
                                        DeltaX_Update = Vehicles[n].VehFrames[f].SpaceHeadway + ( Vehicles[n].VehFrames[f].LocalY - Xt_Update );
                                        DeltaV_Update = Vehicles[n].VehFrames[f].RelativeVelocity + ( Vt_Update - Vehicles[n].VehFrames[f].VehicleVelocity );
                                        S_Update = Vehicles[n].VehFrames[f].SpaceGap + ( Vehicles[n].VehFrames[f].LocalY - Xt_Update );//Possible Error: Problem Since Space Gaps are Sometimes Negative: Use Space Headway Instead
                                        
                                        if (f != 0)//Not First Reading
                                            Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame = Vehicles[n].VehFrames[f-1].LeadersChangingLanes_Frame + 1;
                                        
                                    }
                                    //End of Added by Samer Hamdar: 8 October 2008
                                }
                                else
                                {
                                    At_Update = Vehicles[n].VehFrames[f-1].WallstenAcceleration;//Since at this step, no Wallsten Acceleration has been created --> taken from previous time step
                                    Vt_Update = Vehicles[n].VehFrames[f-1].WallstenVelocity;
                                    Xt_Update = Vehicles[n].VehFrames[f-1].WallstenX;
                                    DeltaX_Update = Vehicles[n].VehFrames[f-1].WallstenSpaceHeadway;
                                    DeltaV_Update = Vehicles[n].VehFrames[f-1].WallstenRelativeSpeed;
                                    S_Update = Vehicles[n].VehFrames[f-1].WallstenGap;
                                }
                                
                                // Test: Seff: Martin's Version: Keep for Updating Purposes: Keep Errors Defined
                                //if ( ( S_Update - So_D ) > 0.0 )
                                if ( ( S_Update - So_D ) > 0.1 )
                                    Seff = ( S_Update - So_D );
                                else
                                    Seff = 0.1;
                                //Seff = 0.0;
                                
                                
                                //Test: Seff: Modified Conservative Version: Minimum Seff = S0 = 3 and Use Space Headway Instead of Gap
                                //if ( ( DeltaX_Update - So_D ) > 3.0 )
                                //    Seff = ( DeltaX_Update - So_D );
                                //else
                                //    Seff = 3.0;
                                
                                
                                ////Test
                                //if (DeltaV_Update == 0.0)
                                //    DeltaV_Update = 0.1;
                                
                                ////Test
                                //if (Vt_Update < 0.1)
                                //    Vt_Update = 0.1;
                                
                                //Tau
                                if ( DeltaV_Update > ( Seff / Tmax_D ) )
                                    Tau =  ( Seff / DeltaV_Update );
                                else
                                    Tau = Tmax_D;
                                
                                
                                //Zprime
                                Zprime = ( Tau / ( 2.0 * Alpha_D * Vt_Update ) );
                                
                                //Zdoubleprime
                                Zdoubleprime = 0.0;
                                
                                
                                //Zstar
                                Zstar = ( - sqrt ( 2.0 * log ( ( Ao_D * Wc_D * Zprime ) / ( sqrt ( 2.0 * PI ) ) ) ) );
                                
                                //Astar : Initial Estimate
                                Astar = ( ( 2.0 / Tau) * ( ( Seff / Tau) - DeltaV_Update + ( Alpha_D * Vt_Update * Zstar ) ) );
                                
                                //Newton's Method loop
                                for ( int NewtonCounter = 0; NewtonCounter < 4 ; NewtonCounter++ )
                                {
                                    X = ( Astar / Ao_D );
                                    
                                    /*G = ( X / ( pow ( ( 1 + pow ( X , 2.0 ) ) , Sigma_D ) ) );
                                    
                                    Gprime = ( ( 1.0 / ( pow ( ( 1.0 + pow ( X , 2.0 )), Gamma_D ) ) ) - ( ( 2.0 * Gamma_D * pow ( X , 2.0 ) ) / ( pow ( ( 1 + pow ( X , 2.0 ) ) ,( Gamma_D + 1.0) ) ) ) );
                                    Gdoubleprime = ( ( ( - 6.0 * Gamma_D * X ) / ( pow ( ( 1.0 + pow ( X , 2.0 ) ) ,( Gamma_D + 1) ) ) ) + ( ( 4.0 * Gamma_D * ( Gamma_D + 1 ) * pow ( X , 3.0) ) / ( pow ( ( 1 + pow ( X , 2.0 ) ) ,( Gamma_D + 2.0) ) ) ) );
                                    
                                    Uptprime = ( ( 1.0 / Ao_D ) *  ( ( ( Wm_D + ( ( DeltaW_D * ( 1 + tanh ( X ) ) ) / PSI_D ) ) * ( Gprime) ) + ( ( DeltaW_D * G ) / ( PSI_D * pow ( cosh ( X ) , 2 ) ) ) ) );
                                    Uptdoubleprime = ( ( 1.0 / ( pow (Ao_D , 2.0 ) ) ) *  ( ( ( Wm_D + (  (  DeltaW_D * ( 1 + tanh ( X ) ) ) / PSI_D ) ) * ( Gdoubleprime) ) + ( ( ( DeltaW_D ) / ( PSI_D * pow ( cosh ( X ) , 2.0 ) ) ) * ( Gprime - ( tanh ( X ) * G ) ) ) ) );
                                    */
                                    
                                    if (X >= 0)
                                     {
                                     Uptprime = Gamma1_D * pow(X, Gamma1_D - 1);
                                     Uptdoubleprime = Gamma1_D * (Gamma1_D - 1) * pow(X, Gamma1_D - 2);
                                     }
                                     else
                                     {
                                     Uptprime = Wm_D * Gamma2_D * pow(-X, Gamma2_D - 1);
                                     Uptdoubleprime = -Wm_D * Gamma2_D * (Gamma2_D - 1) * pow(-X, Gamma2_D - 2);
                                     }
                                    
                                    /*if (Astar >= 0)
                                    {
                                        Uptprime = pow(Astar, -Gamma1_D);
                                        Uptdoubleprime = -Gamma1_D * pow(Astar, -Gamma1_D - 1);
                                    }
                                    else
                                    {
                                        Uptprime = pow(-Astar, -Gamma2_D);
                                        Uptdoubleprime = Gamma2_D * pow(-Astar, -Gamma2_D - 1);
                                    }*/
                                    
                                    /*if (Astar >= 0)
                                     {
                                     Uptprime = -(1 - Gamma1_D) * pow(-Astar, -Gamma1_D) * exp(-Wm_D * pow(-Astar, 1 - Gamma1_D));
                                     Uptdoubleprime = -(1 - Gamma1_D) * -Gamma1_D * pow(-Astar, -Gamma1_D - 1) * exp(-Wm_D * pow(-Astar, 1 - Gamma1_D)) -
                                     pow(((1 - Gamma1_D) * pow(-Astar, -Gamma1_D)), 2) * exp(-Wm_D * pow(-Astar, 1 - Gamma1_D));
                                     }
                                     else
                                     {
                                     Uptprime = (1 - Gamma2_D) * pow(-Astar, -Gamma2_D) * exp(-Wm_D * pow(-Astar, 1 - Gamma2_D));
                                     Uptdoubleprime = (1 - Gamma1_D) * -Gamma1_D * pow(-Astar, -Gamma1_D - 1) * exp(-Wm_D * pow(-Astar, 1 - Gamma1_D)) +
                                     pow(((1 - Gamma1_D) * pow(-Astar, -Gamma1_D)), 2) * exp(-Wm_D * pow(-Astar, 1 - Gamma1_D));
                                     }*/
                                    
                                    Z = ( ( DeltaV_Update + ( ( 0.5 ) * Astar * Tau ) - ( Seff / Tau) ) / ( Alpha_D * Vt_Update ) );
                                    fn = ( 1.0 / sqrt ( 2 * PI) ) * exp (  - pow ( Z , 2.0) / 2.0 );
                                    
                                    F = Uptprime - Wc_D * fn * Zprime;
                                    Fprime = Uptdoubleprime + Wc_D * fn * ( Z * pow ( Zprime, 2.0) + Zdoubleprime );
                                    
                                    Astar = Astar - ( F / Fprime);
                                }
                                
                                //Compute U'' or F' for the last time for variance computation
                                X = ( Astar / Ao_D );
                                
                                /*G = ( X / ( pow ( ( 1.0 + pow ( X , 2.0 ) ) , Sigma_D ) ) );
                                
                                Gprime = ( ( 1.0 / ( pow ( ( 1 + pow ( X , 2.0 )), Gamma_D ) ) ) - ( ( 2.0 * Gamma_D * pow ( X , 2.0 ) ) / ( pow ( ( 1 + pow ( X , 2.0 ) ) ,( Gamma_D + 1.0) ) ) ) );
                                Gdoubleprime = ( ( ( - 6.0 * Gamma_D * X ) / ( pow ( ( 1.0 + pow ( X , 2.0 ) ) ,( Gamma_D + 1.0) ) ) ) + ( ( 4.0 * Gamma_D * ( Gamma_D + 1.0 ) * pow ( X , 3.0) ) / ( pow ( ( 1.0 + pow ( X , 2.0 ) ) ,( Gamma_D + 2.0) ) ) ) );
                                
                                Uptprime = ( ( 1.0 / Ao_D ) *  ( ( ( Wm_D + ( ( DeltaW_D * ( 1 + tanh ( X ) ) ) / PSI_D ) ) * ( Gprime) ) + ( ( DeltaW_D * G ) / ( PSI_D * pow ( cosh ( X ) , 2 ) ) ) ) );
                                Uptdoubleprime = ( ( 1.0 / ( pow (Ao_D , 2.0 ) ) ) *  ( ( ( Wm_D + (  (  DeltaW_D * ( 1 + tanh ( X ) ) ) / PSI_D ) ) * ( Gdoubleprime) ) + ( ( ( DeltaW_D ) / ( PSI_D * pow ( cosh ( X ) , 2.0 ) ) ) * ( Gprime - ( tanh ( X ) * G ) ) ) ) );
                                */
                                
                                if (X >= 0)
                                 {
                                 Uptprime = Gamma1_D * pow(X, Gamma1_D - 1);
                                 Uptdoubleprime = Gamma1_D * (Gamma1_D - 1) * pow(X, Gamma1_D - 2);
                                 }
                                 else
                                 {
                                 Uptprime = Wm_D * Gamma2_D * pow(-X, Gamma2_D - 1);
                                 Uptdoubleprime = -Wm_D * Gamma2_D * (Gamma2_D - 1) * pow(-X, Gamma2_D - 2);
                                 }
                                
                                /*if (Astar >= 0)
                                {
                                    Uptprime = pow(Astar, -Gamma1_D);
                                    Uptdoubleprime = -Gamma1_D * pow(Astar, -Gamma1_D - 1);
                                }
                                else
                                {
                                    Uptprime = pow(-Astar, -Gamma2_D);
                                    Uptdoubleprime = Gamma2_D * pow(-Astar, -Gamma2_D - 1);
                                }*/
                                
                                /*if (Astar >= 0)
                                 {
                                 Uptprime = -(1 - Gamma1_D) * pow(-Astar, -Gamma1_D) * exp(-Wm_D * pow(-Astar, 1 - Gamma1_D));
                                 Uptdoubleprime = -(1 - Gamma1_D) * -Gamma1_D * pow(-Astar, -Gamma1_D - 1) * exp(-Wm_D * pow(-Astar, 1 - Gamma1_D)) -
                                 pow(((1 - Gamma1_D) * pow(-Astar, -Gamma1_D)), 2) * exp(-Wm_D * pow(-Astar, 1 - Gamma1_D));
                                 }
                                 else
                                 {
                                 Uptprime = (1 - Gamma2_D) * pow(-Astar, -Gamma2_D) * exp(-Wm_D * pow(-Astar, 1 - Gamma2_D));
                                 Uptdoubleprime = (1 - Gamma1_D) * -Gamma1_D * pow(-Astar, -Gamma1_D - 1) * exp(-Wm_D * pow(-Astar, 1 - Gamma1_D)) +
                                 pow(((1 - Gamma1_D) * pow(-Astar, -Gamma1_D)), 2) * exp(-Wm_D * pow(-Astar, 1 - Gamma1_D));
                                 }*/
                                
                                Z = ( ( DeltaV_Update + ( ( 0.5 ) * Astar * Tau ) - ( Seff / Tau) ) / ( Alpha_D * Vt_Update ) );
                                fn = ( 1.0 / sqrt ( 2 * PI ) ) * exp (  - pow ( Z , 2.0) / 2.0 );
                                
                                F = Uptprime - Wc_D * fn * Zprime;
                                Fprime = Uptdoubleprime + Wc_D * fn * ( Z * pow ( Zprime, 2.0) + Zdoubleprime );
                                
                                
                                //Var (Variance)
                                
                                Var = -1.0 / ( Beta_D * Fprime );
                                
                                //Wiener Process (Problem?)
                                
                                Random_Wiener = ( (rand()%1000) / 1000.0 );
                                Yt_previous = Yt;
                                
                                Yt = Yt * exp ( - DeltaT / Tau) + sqrt ( 24.0 * DeltaT / Tau ) * Random_Wiener;
                                
                                At_Update = Astar + Var * Yt;
                                
                                //_______________________________Added by Samer Hamdar: 13 October 2008
                                
                                At_freeflow = ( Amax_D * ( 1 - ( Vt_Update / Vo_D ) ) );
                                
                                //Compute Variables for Hazard Computation
                                
                                AverageSurroundingDensitytmp = 0.0;
                                AverageSurroundingDensityLaneRtmp = 0.0;
                                AverageSurroundingDensityLaneLtmp = 0.0;
                                
                                x = 0;//to track frametime data
                                y = 0;//to track frametime.framelocation data
                                
                                while (x < t) //Search for the Frame Limiting The Episode in the Sorted Data in Time Level
                                {
                                    y = 0;// Initialize y to go through FrameLocations Corresponding to Frame Time Slot x
                                    
                                    while (y < FrameTime[x].NumFramesLocation)//Search for the Frame Limiting The Episode in the Sorted Data in Space Level
                                    {
                                        if (FrameTime[x].FrameLocation[y].FrameGlobalID == Vehicles[n].VehFrames[f].FrameGlobalID)
                                        {
                                            
                                            x_c = x;
                                            y_c = y;
                                            
                                            //Speed
                                            Vehicles[n].VehFrames[f].DriverSpeed_Frame = Vt_Update;
                                            
                                            //Distance_To_Ramp
                                            Vehicles[n].VehFrames[f].DistanceToRamp_Frame = fabs ( RampLength - Xt_Update);
                                            
                                            
                                            //First Leader Related Data (Same Lane)
                                            sw = FrameTime[x].NumFramesLocation - y;
                                            
                                            dummy = 0;
                                            for (int w = 1; w < sw; w++)
                                            {
                                                if (FrameTime[x].FrameLocation[y].Lane == FrameTime[x].FrameLocation[y+w].Lane)
                                                {
                                                    dummy = 1;
                                                    Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame = (Vt_Update - FrameTime[x].FrameLocation[y+w].VehicleVelocity);
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayLeader1_Frame = (FrameTime[x].FrameLocation[y+w].LocalY - Xt_Update);
                                                    w = sw+1;
                                                }
                                            }
                                            if (dummy == 0)//No First Leader
                                            {
                                                if ( FrameTime[x].FrameLocation[y].Lane == 7 )
                                                {
                                                    Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame = Vt_Update;
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayLeader1_Frame = ( RampLength - Xt_Update);
                                                }
                                                else
                                                {
                                                    Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame = -1.1234;
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayLeader1_Frame = 20.1234;
                                                    
                                                    Vehicles[n].VehFrames[f].Flag1 = 1;
                                                }
                                            }
                                            
                                            //Second Leader Related Data (Same Lane)
                                            dummy = 0;
                                            track = 2;
                                            for (int w = 1; w < sw; w++)
                                            {
                                                if (FrameTime[x].FrameLocation[y].Lane == FrameTime[x].FrameLocation[y+w].Lane)
                                                {
                                                    track --;
                                                    if (track == 0)
                                                    {
                                                        dummy = 1;
                                                        Vehicles[n].VehFrames[f].RelativeSpeedLeader2_Frame = ( Vt_Update - FrameTime[x].FrameLocation[y+w].VehicleVelocity);
                                                        Vehicles[n].VehFrames[f].DistanceHeadwayLeader2_Frame = (FrameTime[x].FrameLocation[y+w].LocalY - Xt_Update);
                                                        w = sw+1;
                                                    }
                                                }
                                            }
                                            if (dummy == 0)//No Second Leader
                                            {
                                                Vehicles[n].VehFrames[f].RelativeSpeedLeader2_Frame = -1.1234;
                                                Vehicles[n].VehFrames[f].DistanceHeadwayLeader2_Frame = 35.1234;
                                                
                                                Vehicles[n].VehFrames[f].Flag1 = 1;
                                            }
                                            
                                            //First Follower Related Data (Same Lane)
                                            dummy = 0;
                                            for (int w = 1; w <= y; w++)
                                            {
                                                if (FrameTime[x].FrameLocation[y].Lane == FrameTime[x].FrameLocation[y-w].Lane)
                                                {
                                                    dummy = 1;
                                                    Vehicles[n].VehFrames[f].RelativeSpeedFollower1_Frame = (Vt_Update - FrameTime[x].FrameLocation[y-w].VehicleVelocity);
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayFollower1_Frame = (Xt_Update - FrameTime[x].FrameLocation[y-w].LocalY);
                                                    w = y+1;
                                                }
                                            }
                                            if (dummy == 0)//No First Follower
                                            {
                                                Vehicles[n].VehFrames[f].RelativeSpeedFollower1_Frame = 1.1234;
                                                Vehicles[n].VehFrames[f].DistanceHeadwayFollower1_Frame = 20.1234;
                                                
                                                Vehicles[n].VehFrames[f].Flag1 = 1;
                                            }
                                            
                                            //First Leader Related Data (Left Lane)
                                            sw = FrameTime[x].NumFramesLocation - y;
                                            dummy = 0;
                                            for (int w = 1; w < sw; w++)
                                            {
                                                if (FrameTime[x].FrameLocation[y].Lane == (FrameTime[x].FrameLocation[y+w].Lane + 1))
                                                {
                                                    dummy = 1;
                                                    Vehicles[n].VehFrames[f].RelativeSpeedLeftL1_Frame = ( Vt_Update - FrameTime[x].FrameLocation[y+w].VehicleVelocity);
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayLeftL1_Frame = (FrameTime[x].FrameLocation[y+w].LocalY - Xt_Update);
                                                    w = sw+1;
                                                }
                                            }
                                            if (dummy == 0)//No First Leader / (Lane 1) (NB: Data related to Lane 9 - shoulder are ignored) + (Lane Seven Always Consider Headway Ahead)
                                            {
                                                //veh_duration_data_SUB.RelativeSpeedLeftL1 = 999;
                                                //veh_duration_data_SUB.DistanceHeadwayLeftL1 = 9999;
                                                if ( FrameTime[x].FrameLocation[y].Lane == 1 )
                                                {
                                                    Vehicles[n].VehFrames[f].RelativeSpeedLeftL1_Frame = 30.1234;
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayLeftL1_Frame = 0.1234;
                                                }
                                                else
                                                {
                                                    Vehicles[n].VehFrames[f].RelativeSpeedLeftL1_Frame = -1.1234;
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayLeftL1_Frame = 20.1234;
                                                    
                                                    Vehicles[n].VehFrames[f].Flag1 = 1;
                                                }
                                            }
                                            
                                            //First Leader Related Data (Right Lane)
                                            dummy = 0;
                                            for (int w = 1; w < sw; w++)
                                            {
                                                if (FrameTime[x].FrameLocation[y].Lane == (FrameTime[x].FrameLocation[y+w].Lane - 1) && FrameTime[x].FrameLocation[y].Lane != 6)
                                                {
                                                    dummy = 1;
                                                    Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame = (Vt_Update - FrameTime[x].FrameLocation[y+w].VehicleVelocity);
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame = (FrameTime[x].FrameLocation[y+w].LocalY - Xt_Update);
                                                    w = sw+1;
                                                }
                                                else if (  FrameTime[x].FrameLocation[y].Lane == 6  )
                                                {
                                                    if ( FrameTime[x].FrameLocation[y].LocalY > 91 && FrameTime[x].FrameLocation[y].LocalY < 215)
                                                    {
                                                        dummy = 1;
                                                        Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame = (Vt_Update - FrameTime[x].FrameLocation[y+w].VehicleVelocity);
                                                        Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame = (FrameTime[x].FrameLocation[y+w].LocalY - Xt_Update);
                                                        w = sw+1;
                                                        
                                                        Vehicles[n].VehFrames[f].Flag2 = 1;
                                                    }
                                                    else
                                                    {
                                                        dummy = 1;
                                                        Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame = 30.1234;
                                                        Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame = 0.1234;
                                                    }
                                                    
                                                }
                                            }
                                            if (dummy == 0)//No First Leader / Lane 7/ (NB: Data related to Lane 9 - shoulder are ignored)
                                            {
                                                //veh_duration_data_SUB.RelativeSpeedRightL1 = 999;
                                                //veh_duration_data_SUB.DistanceHeadwayRightL1 = 9999;
                                                if ( FrameTime[x].FrameLocation[y].Lane == 7 )
                                                {
                                                    Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame = 30.1234;
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame = 0.1234;
                                                }
                                                else
                                                {
                                                    Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame = -1.1234;
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame = 20.1234;
                                                    
                                                    Vehicles[n].VehFrames[f].Flag1 = 1;
                                                }
                                            }
                                            
                                            
                                            //First Follower Related Data (Left Lane)
                                            dummy = 0;
                                            for (int w = 1; w <= y; w++)
                                            {
                                                if (FrameTime[x].FrameLocation[y].Lane == (FrameTime[x].FrameLocation[y-w].Lane + 1))
                                                {
                                                    dummy = 1;
                                                    Vehicles[n].VehFrames[f].RelativeSpeedLeftF1_Frame =(Vt_Update - FrameTime[x].FrameLocation[y-w].VehicleVelocity);
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayLeftF1_Frame = (Xt_Update - FrameTime[x].FrameLocation[y-w].LocalY);
                                                    w = y+1;
                                                }
                                            }
                                            if (dummy == 0)//No First Follower / (Lane 1)
                                            {
                                                //veh_duration_data_SUB.RelativeSpeedLeftF1 = 999;
                                                //veh_duration_data_SUB.DistanceHeadwayLeftF1 = 9999;
                                                if ( FrameTime[x].FrameLocation[y].Lane == 1 )
                                                {
                                                    Vehicles[n].VehFrames[f].RelativeSpeedLeftF1_Frame = -30.1234;
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayLeftF1_Frame = 0.1234;
                                                }
                                                else
                                                {
                                                    Vehicles[n].VehFrames[f].RelativeSpeedLeftF1_Frame = 1.1234;
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayLeftF1_Frame = 20.1234;
                                                    
                                                    Vehicles[n].VehFrames[f].Flag1 = 1;
                                                }
                                            }
                                            
                                            //First Follower Related Data (Right Lane)
                                            dummy = 0;
                                            for (int w = 1; w <= y; w++)
                                            {
                                                if (FrameTime[x].FrameLocation[y].Lane == (FrameTime[x].FrameLocation[y-w].Lane - 1) && FrameTime[x].FrameLocation[y].Lane != 6)
                                                {
                                                    dummy = 1;
                                                    Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame = (Vt_Update - FrameTime[x].FrameLocation[y-w].VehicleVelocity);
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame = (Xt_Update - FrameTime[x].FrameLocation[y-w].LocalY);
                                                    w = y+1;
                                                }
                                                else if ( FrameTime[x].FrameLocation[y].Lane == 6 )
                                                {
                                                    if ( FrameTime[x].FrameLocation[y].LocalY > 91 && FrameTime[x].FrameLocation[y].LocalY < 215)
                                                    {
                                                        dummy = 1;
                                                        Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame = (Vt_Update - FrameTime[x].FrameLocation[y-w].VehicleVelocity);
                                                        Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame = (Xt_Update - FrameTime[x].FrameLocation[y-w].LocalY);
                                                        w = y+1;
                                                        
                                                        Vehicles[n].VehFrames[f].Flag2 = 1;
                                                    }
                                                    else
                                                    {
                                                        dummy = 1;
                                                        Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame = -30.1234;
                                                        Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame = 0.1234;
                                                    }
                                                }
                                            }
                                            if (dummy == 0)//No First Follower / (Lane 7)
                                            {
                                                //veh_duration_data_SUB.RelativeSpeedRightF1 = 999;
                                                //veh_duration_data_SUB.DistanceHeadwayRightF1 = 9999;
                                                if ( FrameTime[x].FrameLocation[y].Lane == 7 )
                                                {
                                                    Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame = - 30.1234;
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame = 0.1234;
                                                }
                                                else
                                                {
                                                    Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame = 1.1234;
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame = 20.1234;
                                                    
                                                    Vehicles[n].VehFrames[f].Flag1 = 1;
                                                }
                                            }
                                            
                                            //Average Density Related Data
                                            AverageSurroundingDensitytmp = 1;//Modified by Samer Hamdar, April 5, 2007: the vehicle of interest is considered in the surrounding density
                                            AverageSurroundingDensityLaneRtmp = 0;
                                            AverageSurroundingDensityLaneLtmp = 0;
                                            
                                            //General
                                            ww = 1;
                                            //Ahead
                                            while ( ww < sw)
                                            {
                                                if ( FrameTime[x].FrameLocation[y + ww].LocalY - Xt_Update <= DistanceDensityAhead)
                                                {
                                                    AverageSurroundingDensitytmp = AverageSurroundingDensitytmp + 1;
                                                }
                                                ww = ww + 1;
                                            }
                                            
                                            //Behind
                                            ww = 1;
                                            while (ww <= y)
                                            {
                                                if (  Xt_Update  - FrameTime[x].FrameLocation[y - ww].LocalY <= DistanceDensityBehind)
                                                {
                                                    AverageSurroundingDensitytmp = AverageSurroundingDensitytmp + 1;
                                                }
                                                ww = ww + 1;
                                            }
                                            
                                            //Interpolation
                                            if ((Xt_Update < DistanceDensityBehind) && (L - Xt_Update >= DistanceDensityAhead))
                                            {
                                                AverageSurroundingDensitytmp = (AverageSurroundingDensitytmp * (DistanceDensityAhead + DistanceDensityBehind) / (Xt_Update + DistanceDensityAhead));
                                            }
                                            
                                            if ((Xt_Update >= DistanceDensityBehind) && (L - Xt_Update < DistanceDensityAhead))
                                            {
                                                AverageSurroundingDensitytmp = (AverageSurroundingDensitytmp * (DistanceDensityAhead + DistanceDensityBehind) / (DistanceDensityBehind + L - Xt_Update));
                                            }
                                            
                                            if ((Xt_Update < DistanceDensityBehind) && (L - Xt_Update < DistanceDensityAhead))
                                            {
                                                AverageSurroundingDensitytmp = (AverageSurroundingDensitytmp * (DistanceDensityAhead + DistanceDensityBehind) / L);
                                            }
                                            
                                            AverageSurroundingDensitytmp = ((AverageSurroundingDensitytmp * 1000.0)/(DistanceDensityAhead + DistanceDensityBehind) / NLanes);
                                            Vehicles[n].VehFrames[f].AverageSurroundingDensity_Frame = AverageSurroundingDensitytmp;
                                            
                                            
                                            //Average Density to the Right Related Data
                                            ww = 1;
                                            //Ahead
                                            while ( ww < sw)
                                            {
                                                if ((FrameTime[x].FrameLocation[y + ww].LocalY - Xt_Update <= DistanceDensityAhead) && (FrameTime[x].FrameLocation[y].Lane == (FrameTime[x].FrameLocation[y + ww].Lane - 1) ) )
                                                {
                                                    AverageSurroundingDensityLaneRtmp = AverageSurroundingDensityLaneRtmp + 1;
                                                }
                                                ww = ww + 1;
                                            }
                                            
                                            //Behind
                                            ww = 1;
                                            while (ww <= y)
                                            {
                                                if (  Xt_Update  - FrameTime[x].FrameLocation[y - ww].LocalY <= DistanceDensityBehind && (FrameTime[x].FrameLocation[y].Lane == (FrameTime[x].FrameLocation[y-ww].Lane - 1)))
                                                {
                                                    AverageSurroundingDensityLaneRtmp = AverageSurroundingDensityLaneRtmp + 1;
                                                }
                                                ww = ww + 1;
                                            }
                                            
                                            //Interpolation
                                            if ((Xt_Update < DistanceDensityBehind) && (L - Xt_Update >= DistanceDensityAhead))
                                            {
                                                AverageSurroundingDensityLaneRtmp = (AverageSurroundingDensityLaneRtmp * (DistanceDensityAhead + DistanceDensityBehind) / (Xt_Update + DistanceDensityAhead));
                                            }
                                            
                                            if ((Xt_Update >= DistanceDensityBehind) && (L - Xt_Update < DistanceDensityAhead))
                                            {
                                                AverageSurroundingDensityLaneRtmp = (AverageSurroundingDensityLaneRtmp * (DistanceDensityAhead + DistanceDensityBehind) / (DistanceDensityBehind + L - Xt_Update));
                                            }
                                            
                                            if ((Xt_Update < DistanceDensityBehind) && (L - Xt_Update < DistanceDensityAhead))
                                            {
                                                AverageSurroundingDensityLaneRtmp = (AverageSurroundingDensityLaneRtmp * (DistanceDensityAhead + DistanceDensityBehind) / L);
                                            }
                                            
                                            AverageSurroundingDensityLaneRtmp = ((AverageSurroundingDensityLaneRtmp * 1000.0)/(DistanceDensityAhead + DistanceDensityBehind));// 1 mile = 5280 fts
                                            
                                            
                                            if (FrameTime[x].FrameLocation[y].Lane == 6)
                                            {
                                                if ( Xt_Update > 91 && Xt_Update < 215)
                                                {
                                                    AverageSurroundingDensityLaneRtmp = AverageSurroundingDensityLaneRtmp;
                                                }
                                                else
                                                {
                                                    AverageSurroundingDensityLaneRtmp = 100.1234;
                                                    
                                                }
                                            }
                                            
                                            if (FrameTime[x].FrameLocation[y].Lane == 7)
                                            {
                                                AverageSurroundingDensityLaneRtmp = 100.1234;
                                            }
                                            
                                            Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneR_Frame = AverageSurroundingDensityLaneRtmp;
                                            
                                            //Average Density to the Left Related Data
                                            ww = 1;
                                            //Ahead
                                            while ( ww < sw)
                                            {
                                                if ((FrameTime[x].FrameLocation[y + ww].LocalY - Xt_Update <= DistanceDensityAhead) && (FrameTime[x].FrameLocation[y].Lane == (FrameTime[x].FrameLocation[y + ww].Lane + 1)) )
                                                {
                                                    AverageSurroundingDensityLaneLtmp = AverageSurroundingDensityLaneLtmp + 1;
                                                }
                                                ww = ww + 1;
                                            }
                                            
                                            //Behind
                                            ww = 1;
                                            while (ww <= y)
                                            {
                                                if (  ( Xt_Update  - FrameTime[x].FrameLocation[y-ww].LocalY <= DistanceDensityBehind ) && ( FrameTime[x].FrameLocation[y].Lane == (FrameTime[x].FrameLocation[y-ww].Lane + 1) ) )
                                                {
                                                    AverageSurroundingDensityLaneLtmp = AverageSurroundingDensityLaneLtmp + 1;
                                                }
                                                ww = ww + 1;
                                            }
                                            
                                            //Interpolation
                                            if ((Xt_Update < DistanceDensityBehind) && (L - Xt_Update >= DistanceDensityAhead))
                                            {
                                                AverageSurroundingDensityLaneLtmp = (AverageSurroundingDensityLaneLtmp * (DistanceDensityAhead + DistanceDensityBehind) / (Xt_Update + DistanceDensityAhead));
                                            }
                                            
                                            if ((Xt_Update >= DistanceDensityBehind) && (L - Xt_Update < DistanceDensityAhead))
                                            {
                                                AverageSurroundingDensityLaneLtmp = (AverageSurroundingDensityLaneLtmp * (DistanceDensityAhead + DistanceDensityBehind) / (DistanceDensityBehind + L - Xt_Update));
                                            }
                                            
                                            if ((Xt_Update < DistanceDensityBehind) && (L - Xt_Update < DistanceDensityAhead))
                                            {
                                                AverageSurroundingDensityLaneLtmp = (AverageSurroundingDensityLaneLtmp * (DistanceDensityAhead + DistanceDensityBehind) / L);
                                                
                                            }
                                            
                                            AverageSurroundingDensityLaneLtmp = ((AverageSurroundingDensityLaneLtmp * 1000.0)/(DistanceDensityAhead + DistanceDensityBehind));// 1 mile = 5280 fts
                                            
                                            if (FrameTime[x].FrameLocation[y].Lane == 1)
                                            {
                                                AverageSurroundingDensityLaneLtmp = 100.1234;
                                            }
                                            
                                            Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneL_Frame = AverageSurroundingDensityLaneLtmp;
                                            
                                            break;
                                        }
                                        y++;
                                    }
                                    x++;
                                }
                                
                                //Hazard: Car-Following wih Lane Change Exit
                                
                                //__Assuming Weibull Distribution
                                
                                //____Number One: Limdep Technique
                                Lambda_tmp = exp ( -1.0 * ( BetaOne_CF_LC + BetaLCL_CF_LC * Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame + BetaV_CF_LC * Vehicles[n].VehFrames[f].DriverSpeed_Frame + BetaDXL1_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeader1_Frame + BetaDVL1_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame + BetaDXL2_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeader2_Frame + BetaDVL2_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeader2_Frame + BetaDXF1_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayFollower1_Frame + BetaDVF1_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedFollower1_Frame + BetaDXL1R_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame + BetaDVL1R_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame + BetaDXF1R_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame + BetaDVF1R_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame + BetaDXL1L_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeftL1_Frame + BetaDVL1L_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeftL1_Frame + BetaDXF1L_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeftF1_Frame + BetaDVF1L_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeftF1_Frame + BetaKR_CF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneR_Frame + BetaKL_CF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneL_Frame + BetaK_CF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensity_Frame + BetaRightLane_CF_LC * Vehicles[n].VehFrames[f].RightLaneDummy_Frame + BetaLeftLane_CF_LC * Vehicles[n].VehFrames[f].LeftLaneDummy_Frame + BetaRamp_CF_LC * Vehicles[n].VehFrames[f].RampDummy_Frame + BetaDistanceToRamp_CF_LC * Vehicles[n].VehFrames[f].DistanceToRamp_Frame) );
                                
                                Hazard_ParameterA_CF_LC = Lambda_tmp;
                                Hazard_tmp = Hazard_ParameterA_CF_LC * Hazard_ParameterB_CF_LC * pow ( ( Hazard_ParameterA_CF_LC * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ),( Hazard_ParameterB_CF_LC - 1 ) );
                                Survival_tmp = exp (  -1.0 * pow ( ( Hazard_ParameterA_CF_LC * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ) , Hazard_ParameterB_CF_LC )  );
                                
                                R_Heterogeneity_CF_LC = K_Heterogeneity_CF_LC;
                                
                                //Teta_tmp = ( 1.0 / K_Heterogeneity_CF_LC );
                                Teta_tmp = 0.55243252;
                                
                                Survival2_tmp = Survival_tmp * Teta_tmp;
                                
                                Hazard2_tmp = Survival2_tmp * Hazard_tmp;
                                
                                //____Number Two: Bhat Technique
                                //R_Heterogeneity_CF_LC = K_Heterogeneity_CF_LC;
                                //Hazard_tmp = Hazard_ParameterA_CF_LC * Hazard_ParameterB_CF_LC * pow ( ( Hazard_ParameterA_CF_LC * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ),( Hazard_ParameterB_CF_LC - 1 ) );
                                //Survival_tmp = exp (  -1.0 * pow ( ( Hazard_ParameterA_CF_LC * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ) , Hazard_ParameterB_CF_LC )  );
                                //
                                ////Wrong:Gamma_tmp = ( pow ( K_Heterogeneity_CF_LC, R_Heterogeneity_CF_LC ) / double ( factorial ( int ( R_Heterogeneity_CF_LC - 1 ) ) ) ) * exp ( - 1.0 * K_Heterogeneity_CF_LC * V_tmp) * pow ( V_tmp , ( R_Heterogeneity_CF_LC - 1)  );
                                //
                                //Gamma_tmp = Vehicles[n].VehFrames[f].GammaHeterogeneity_CF_LC;
                                //W_tmp = log (Gamma_tmp);//(?): What is
                                //Lambda_tmp = exp ( -1.0 * ( BetaOne_CF_LC + BetaLCL_CF_LC * Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame + BetaV_CF_LC * Vehicles[n].VehFrames[f].DriverSpeed_Frame + BetaDXL1_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeader1_Frame + BetaDVL1_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame + BetaDXL2_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeader2_Frame + BetaDVL2_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeader2_Frame + BetaDXF1_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayFollower1_Frame + BetaDVF1_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedFollower1_Frame + BetaDXL1R_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame + BetaDVL1R_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame + BetaDXF1R_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame + BetaDVF1R_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame + BetaDXL1L_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeftL1_Frame + BetaDVL1L_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeftL1_Frame + BetaDXF1L_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeftF1_Frame + BetaDVF1L_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeftF1_Frame + BetaKR_CF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneR_Frame + BetaKL_CF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneL_Frame + BetaK_CF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensity_Frame + BetaRightLane_CF_LC * Vehicles[n].VehFrames[f].RightLaneDummy_Frame + BetaLeftLane_CF_LC * Vehicles[n].VehFrames[f].LeftLaneDummy_Frame + BetaRamp_CF_LC * Vehicles[n].VehFrames[f].RampDummy_Frame + BetaDistanceToRamp_CF_LC * Vehicles[n].VehFrames[f].DistanceToRamp_Frame)+ W_tmp );
                                //Hazard2_tmp = Hazard_tmp * Lambda_tmp;
                                
                                Vehicles[n].VehFrames[f].LaneChange_Hazard_CF = Hazard2_tmp;
                                
                                //Hazard: Car-Following with Mode Exit
                                
                                //__Assuming Weibull Distribution
                                
                                //____Number One: Limdep Technique
                                Lambda_tmp = exp ( -1.0 * ( BetaOne_CF_CM + BetaLCL_CF_CM * Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame + BetaV_CF_CM * Vehicles[n].VehFrames[f].DriverSpeed_Frame + BetaDXL1_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeader1_Frame + BetaDVL1_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame + BetaDXL2_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeader2_Frame + BetaDVL2_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeader2_Frame + BetaDXF1_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayFollower1_Frame + BetaDVF1_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedFollower1_Frame + BetaDXL1R_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame + BetaDVL1R_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame + BetaDXF1R_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame + BetaDVF1R_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame + BetaDXL1L_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeftL1_Frame + BetaDVL1L_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeftL1_Frame + BetaDXF1L_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeftF1_Frame + BetaDVF1L_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeftF1_Frame + BetaKR_CF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneR_Frame + BetaKL_CF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneL_Frame + BetaK_CF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensity_Frame + BetaRightLane_CF_CM * Vehicles[n].VehFrames[f].RightLaneDummy_Frame + BetaLeftLane_CF_CM * Vehicles[n].VehFrames[f].LeftLaneDummy_Frame + BetaRamp_CF_CM * Vehicles[n].VehFrames[f].RampDummy_Frame + BetaDistanceToRamp_CF_CM * Vehicles[n].VehFrames[f].DistanceToRamp_Frame) );
                                
                                Hazard_ParameterA_CF_CM = Lambda_tmp;
                                Hazard_tmp = Hazard_ParameterA_CF_CM * Hazard_ParameterB_CF_CM * pow ( ( Hazard_ParameterA_CF_CM * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ),( Hazard_ParameterB_CF_CM - 1 ) );
                                Survival_tmp = exp (  -1.0 * pow ( ( Hazard_ParameterA_CF_CM * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ) , Hazard_ParameterB_CF_CM )  );
                                
                                R_Heterogeneity_CF_CM = K_Heterogeneity_CF_CM;
                                
                                //Teta_tmp = ( 1.0 / K_Heterogeneity_CF_CM );
                                Teta_tmp = 0.55243252;
                                
                                Survival2_tmp = Survival_tmp * Teta_tmp;
                                
                                Hazard2_tmp = Survival2_tmp * Hazard_tmp;
                                
                                //____Number Two: Bhat Technique
                                //R_Heterogeneity_CF_CM = K_Heterogeneity_CF_CM;
                                //Hazard_tmp = Hazard_ParameterA_CF_CM * Hazard_ParameterB_CF_CM * pow ( ( Hazard_ParameterA_CF_CM * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ),( Hazard_ParameterB_CF_CM - 1 ) );
                                //Survival_tmp = exp (  -1.0 * pow ( ( Hazard_ParameterA_CF_CM * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ) , Hazard_ParameterB_CF_CM )  );
                                //
                                ////Wrong:Gamma_tmp = ( pow ( K_Heterogeneity_CF_CM, R_Heterogeneity_CF_CM ) / double ( factorial ( int ( R_Heterogeneity_CF_CM - 1 ) ) ) ) * exp ( - 1.0 * K_Heterogeneity_CF_CM * V_tmp) * pow ( V_tmp , ( R_Heterogeneity_CF_CM - 1)  );
                                //
                                //Gamma_tmp = Vehicles[n].VehFrames[f].GammaHeterogeneity_CF_CM;
                                //W_tmp = log (Gamma_tmp);//(?): What is
                                //Lambda_tmp = exp ( -1.0 * ( BetaOne_CF_CM + BetaLCL_CF_CM * Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame + BetaV_CF_CM * Vehicles[n].VehFrames[f].DriverSpeed_Frame + BetaDXL1_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeader1_Frame + BetaDVL1_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame + BetaDXL2_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeader2_Frame + BetaDVL2_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeader2_Frame + BetaDXF1_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayFollower1_Frame + BetaDVF1_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedFollower1_Frame + BetaDXL1R_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame + BetaDVL1R_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame + BetaDXF1R_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame + BetaDVF1R_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame + BetaDXL1L_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeftL1_Frame + BetaDVL1L_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeftL1_Frame + BetaDXF1L_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeftF1_Frame + BetaDVF1L_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeftF1_Frame + BetaKR_CF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneR_Frame + BetaKL_CF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneL_Frame + BetaK_CF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensity_Frame + BetaRightLane_CF_CM * Vehicles[n].VehFrames[f].RightLaneDummy_Frame + BetaLeftLane_CF_CM * Vehicles[n].VehFrames[f].LeftLaneDummy_Frame + BetaRamp_CF_CM * Vehicles[n].VehFrames[f].RampDummy_Frame + BetaDistanceToRamp_CF_CM * Vehicles[n].VehFrames[f].DistanceToRamp_Frame)+ W_tmp );
                                //Hazard2_tmp = Hazard_tmp * Lambda_tmp;
                                
                                Vehicles[n].VehFrames[f].Mode_Hazard_CF = Hazard2_tmp;
                                
                                //Hazard: Free Flow with Lane Change Exit
                                
                                //__Assuming Exponential Distribution
                                
                                //____Number One: Limdep Technique
                                Lambda_tmp = exp ( -1.0 * ( BetaOne_FF_LC + BetaLCL_FF_LC * Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame + BetaV_FF_LC * Vehicles[n].VehFrames[f].DriverSpeed_Frame + BetaDXL1_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeader1_Frame + BetaDVL1_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame + BetaDXL2_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeader2_Frame + BetaDVL2_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeader2_Frame + BetaDXF1_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayFollower1_Frame + BetaDVF1_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedFollower1_Frame + BetaDXL1R_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame + BetaDVL1R_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame + BetaDXF1R_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame + BetaDVF1R_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame + BetaDXL1L_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeftL1_Frame + BetaDVL1L_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeftL1_Frame + BetaDXF1L_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeftF1_Frame + BetaDVF1L_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeftF1_Frame + BetaKR_FF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneR_Frame + BetaKL_FF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneL_Frame + BetaK_FF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensity_Frame + BetaRightLane_FF_LC * Vehicles[n].VehFrames[f].RightLaneDummy_Frame + BetaLeftLane_FF_LC * Vehicles[n].VehFrames[f].LeftLaneDummy_Frame + BetaRamp_FF_LC * Vehicles[n].VehFrames[f].RampDummy_Frame + BetaDistanceToRamp_FF_LC * Vehicles[n].VehFrames[f].DistanceToRamp_Frame) );
                                
                                Hazard_ParameterA_FF_LC = Lambda_tmp;
                                Hazard_ParameterB_FF_LC = 1.0;//Difference Between Exponential and Weibull
                                Hazard_tmp = Hazard_ParameterA_FF_LC * Hazard_ParameterB_FF_LC * pow ( ( Hazard_ParameterA_FF_LC * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ),( Hazard_ParameterB_FF_LC - 1 ) );
                                Survival_tmp = exp (  -1.0 * pow ( ( Hazard_ParameterA_FF_LC * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ) , Hazard_ParameterB_FF_LC )  );
                                
                                R_Heterogeneity_FF_LC = K_Heterogeneity_FF_LC;
                                
                                //Teta_tmp = ( 1.0 / K_Heterogeneity_FF_LC );
                                Teta_tmp = 0.55243252;
                                
                                Survival2_tmp = Survival_tmp * Teta_tmp;
                                
                                Hazard2_tmp = Survival2_tmp * Hazard_tmp;
                                
                                //____Number Two: Bhat Technique
                                //R_Heterogeneity_FF_LC = K_Heterogeneity_FF_LC;
                                //Hazard_ParameterB_FF_LC = 1.0;
                                //Hazard_tmp = Hazard_ParameterA_FF_LC * Hazard_ParameterB_FF_LC * pow ( ( Hazard_ParameterA_FF_LC * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ),( Hazard_ParameterB_FF_LC - 1 ) );
                                //Survival_tmp = exp (  -1.0 * pow ( ( Hazard_ParameterA_FF_LC * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ) , Hazard_ParameterB_FF_LC )  );
                                //
                                ////Wrong:Gamma_tmp = ( pow ( K_Heterogeneity_FF_LC, R_Heterogeneity_FF_LC ) / double ( factorial ( int ( R_Heterogeneity_FF_LC - 1 ) ) ) ) * exp ( - 1.0 * K_Heterogeneity_FF_LC * V_tmp) * pow ( V_tmp , ( R_Heterogeneity_FF_LC - 1)  );
                                //
                                ////Gamma_tmp = Vehicles[n].VehFrames[f].GammaHeterogeneity_FF_LC;
                                //W_tmp = log (Gamma_tmp);//(?): What is v
                                //Lambda_tmp = exp ( -1.0 * ( BetaOne_FF_LC + BetaLCL_FF_LC * Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame + BetaV_FF_LC * Vehicles[n].VehFrames[f].DriverSpeed_Frame + BetaDXL1_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeader1_Frame + BetaDVL1_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame + BetaDXL2_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeader2_Frame + BetaDVL2_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeader2_Frame + BetaDXF1_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayFollower1_Frame + BetaDVF1_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedFollower1_Frame + BetaDXL1R_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame + BetaDVL1R_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame + BetaDXF1R_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame + BetaDVF1R_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame + BetaDXL1L_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeftL1_Frame + BetaDVL1L_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeftL1_Frame + BetaDXF1L_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeftF1_Frame + BetaDVF1L_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeftF1_Frame + BetaKR_FF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneR_Frame + BetaKL_FF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneL_Frame + BetaK_FF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensity_Frame + BetaRightLane_FF_LC * Vehicles[n].VehFrames[f].RightLaneDummy_Frame + BetaLeftLane_FF_LC * Vehicles[n].VehFrames[f].LeftLaneDummy_Frame + BetaRamp_FF_LC * Vehicles[n].VehFrames[f].RampDummy_Frame + BetaDistanceToRamp_FF_LC * Vehicles[n].VehFrames[f].DistanceToRamp_Frame)+ W_tmp );
                                //Hazard2_tmp = Hazard_tmp * Lambda_tmp;
                                
                                Vehicles[n].VehFrames[f].LaneChange_Hazard_FF = Hazard2_tmp;
                                
                                //Hazard: Free-Flow with Mode Exit
                                //__Assuming Exponential Distribution
                                
                                //____Number One: Limdep Technique
                                Lambda_tmp = exp ( -1.0 * ( BetaOne_FF_CM + BetaLCL_FF_CM * Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame + BetaV_FF_CM * Vehicles[n].VehFrames[f].DriverSpeed_Frame + BetaDXL1_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeader1_Frame + BetaDVL1_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame + BetaDXL2_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeader2_Frame + BetaDVL2_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeader2_Frame + BetaDXF1_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayFollower1_Frame + BetaDVF1_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedFollower1_Frame + BetaDXL1R_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame + BetaDVL1R_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame + BetaDXF1R_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame + BetaDVF1R_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame + BetaDXL1L_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeftL1_Frame + BetaDVL1L_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeftL1_Frame + BetaDXF1L_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeftF1_Frame + BetaDVF1L_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeftF1_Frame + BetaKR_FF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneR_Frame + BetaKL_FF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneL_Frame + BetaK_FF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensity_Frame + BetaRightLane_FF_CM * Vehicles[n].VehFrames[f].RightLaneDummy_Frame + BetaLeftLane_FF_CM * Vehicles[n].VehFrames[f].LeftLaneDummy_Frame + BetaRamp_FF_CM * Vehicles[n].VehFrames[f].RampDummy_Frame + BetaDistanceToRamp_FF_CM * Vehicles[n].VehFrames[f].DistanceToRamp_Frame) );
                                
                                Hazard_ParameterA_FF_CM = Lambda_tmp;
                                Hazard_ParameterB_FF_CM = 1.0;//Difference Between Exponential and Weibull
                                Hazard_tmp = Hazard_ParameterA_FF_CM * Hazard_ParameterB_FF_CM * pow ( ( Hazard_ParameterA_FF_CM * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ),( Hazard_ParameterB_FF_CM - 1 ) );
                                Survival_tmp = exp (  -1.0 * pow ( ( Hazard_ParameterA_FF_CM * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ) , Hazard_ParameterB_FF_CM )  );
                                
                                R_Heterogeneity_FF_CM = K_Heterogeneity_FF_CM;
                                
                                //Teta_tmp = ( 1.0 / K_Heterogeneity_FF_CM );
                                Teta_tmp = 0.55243252;
                                
                                Survival2_tmp = Survival_tmp * Teta_tmp;
                                
                                Hazard2_tmp = Survival2_tmp * Hazard_tmp;
                                
                                //____Number Two: Bhat Technique
                                //R_Heterogeneity_FF_CM = K_Heterogeneity_FF_CM;
                                //Hazard_ParameterB_FF_CM = 1.0;
                                //Hazard_tmp = Hazard_ParameterA_FF_CM * Hazard_ParameterB_FF_CM * pow ( ( Hazard_ParameterA_FF_CM * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ),( Hazard_ParameterB_FF_CM - 1 ) );
                                //Survival_tmp = exp (  -1.0 * pow ( ( Hazard_ParameterA_FF_CM * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ) , Hazard_ParameterB_FF_CM )  );
                                //
                                ////Wrong:Gamma_tmp = ( pow ( K_Heterogeneity_FF_CM, R_Heterogeneity_FF_CM ) / double ( factorial ( int ( R_Heterogeneity_FF_CM - 1 ) ) ) ) * exp ( - 1.0 * K_Heterogeneity_FF_CM * V_tmp) * pow ( V_tmp , ( R_Heterogeneity_FF_CM - 1)  );
                                //
                                ////Gamma_tmp = Vehicles[n].VehFrames[f].GammaHeterogeneity_FF_CM;
                                //W_tmp = log (Gamma_tmp);//(?): What is v
                                //Lambda_tmp = exp ( -1.0 * ( BetaOne_FF_CM + BetaLCL_FF_CM * Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame + BetaV_FF_CM * Vehicles[n].VehFrames[f].DriverSpeed_Frame + BetaDXL1_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeader1_Frame + BetaDVL1_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame + BetaDXL2_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeader2_Frame + BetaDVL2_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeader2_Frame + BetaDXF1_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayFollower1_Frame + BetaDVF1_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedFollower1_Frame + BetaDXL1R_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame + BetaDVL1R_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame + BetaDXF1R_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame + BetaDVF1R_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame + BetaDXL1L_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeftL1_Frame + BetaDVL1L_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeftL1_Frame + BetaDXF1L_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeftF1_Frame + BetaDVF1L_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeftF1_Frame + BetaKR_FF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneR_Frame + BetaKL_FF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneL_Frame + BetaK_FF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensity_Frame + BetaRightLane_FF_CM * Vehicles[n].VehFrames[f].RightLaneDummy_Frame + BetaLeftLane_FF_CM * Vehicles[n].VehFrames[f].LeftLaneDummy_Frame + BetaRamp_FF_CM * Vehicles[n].VehFrames[f].RampDummy_Frame + BetaDistanceToRamp_FF_CM * Vehicles[n].VehFrames[f].DistanceToRamp_Frame)+ W_tmp );
                                //Hazard2_tmp = Hazard_tmp * Lambda_tmp;
                                
                                Vehicles[n].VehFrames[f].Mode_Hazard_FF = Hazard2_tmp;
                                
                                //Mode Choice: Duration Delimitation
                                Mode_Percentage = ( (rand ()%1000) / 1000.0 );
                                
                                //Test: No Effect if Missing Data
                                if (Vehicles[n].VehFrames[f].Flag1 == 1 || Vehicles[n].VehFrames[f].Flag2 == 1)
                                {
                                    Vehicles[n].VehFrames[f].LaneChange_Hazard_CF = 0.0;
                                    Vehicles[n].VehFrames[f].LaneChange_Hazard_FF = 0.0;
                                    Vehicles[n].VehFrames[f].Mode_Hazard_CF = 0.0;
                                    Vehicles[n].VehFrames[f].Mode_Hazard_FF = 0.0;
                                }
                                
                                {
                                    if (f == 0)
                                    {
                                        //Considered as CF Episode: 1st Episode
                                        if (Mode_Percentage < Vehicles[n].VehFrames[f].Mode_Hazard_CF)//New Episode
                                        {
                                            RT_Counter = 0;
                                            
                                            Vehicles[n].VehFrames[f].Mode = 0;
                                            Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame = 0;
                                            At_Update = At_freeflow;
                                        }
                                        else
                                        {
                                            Vehicles[n].VehFrames[f].Mode = 1;
                                            At_Update = At_Update;
                                        }
                                        
                                    }
                                    else if (Vehicles[n].VehFrames[f].Mode_Previous == 1)
                                    {
                                        //CF Episode
                                        if (Mode_Percentage < Vehicles[n].VehFrames[f].Mode_Hazard_CF)//New Episode
                                        {
                                            RT_Counter = 0;
                                            
                                            Vehicles[n].VehFrames[f].Mode = 0;
                                            Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame = 0;
                                            At_Update = At_freeflow;
                                        }
                                        else
                                        {
                                            Vehicles[n].VehFrames[f].Mode = 1;
                                            At_Update = At_Update;
                                        }
                                    }
                                    else if (Vehicles[n].VehFrames[f].Mode_Previous == 0)
                                    {
                                        //FF Episode
                                        if (Mode_Percentage < Vehicles[n].VehFrames[f].Mode_Hazard_FF)//New Episode
                                        {
                                            RT_Counter = 0;
                                            
                                            Vehicles[n].VehFrames[f].Mode = 1;
                                            Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame = 0;
                                            At_Update = At_Update;
                                        }
                                        else
                                        {
                                            Vehicles[n].VehFrames[f].Mode = 0;
                                            At_Update = At_freeflow;
                                        }
                                    }
                                }
                                
                                //____________________________End of Added by Samer Hamdar: 8 October 2008
                                
                                //Test
                                //Incorporate Free-Speed Contrained Acceleration and Maximum Deceleration (Maximum Acceleration Just in Case
                                //if ( At_Update >  ( Amax_D * ( 1 - ( Vt_Update / Vo_D ) ) ) )
                                //    At_Update =  ( Amax_D * ( 1 - ( Vt_Update / Vo_D ) ) );
                                
                                //if ( At_Update > 4.0 )
                                //    At_Update = 4.0;
                                //if ( At_Update < -6.0 )  //Problem: Test: Put lower limit on acceleration (?)
                                //    At_Update = -6.0;
                                
                                
                                //Find the Resulting Velocity, Relative Velocity, Location, Relative Headway for Updating the Next Acceleration
                                Vt_Update = Vt_Update + ( At_Update * DeltaT );
                                
                                if ( Vt_Update <= 0.0 )//Problem: Test: To Avoid Negative Velocities (?)
                                    Vt_Update = 0.1;
                                
                                Xt_Update = Xt_Update + ( Vt_Update * DeltaT ) + 0.5 * ( At_Update * DeltaT * DeltaT );
                                DeltaX_Update = Vehicles[n].VehFrames[f].SpaceHeadway + ( Vehicles[n].VehFrames[f].LocalY - Xt_Update );
                                DeltaV_Update = Vehicles[n].VehFrames[f].RelativeVelocity + ( Vt_Update - Vehicles[n].VehFrames[f].VehicleVelocity );
                                S_Update = Vehicles[n].VehFrames[f].SpaceGap + ( Vehicles[n].VehFrames[f].LocalY - Xt_Update );//Possible Error: Problem Since Space Gaps are Sometimes Negative: Use Space Headway Instead
                                
                                //To Detect Accidents
                                if (S_Update <= 0)
                                {
                                    S_Update = 0.1;
                                    Vt_Update = 0.1;
                                    At_Update = 0.0;
                                    Vehicles[n].VehParameterChildren_Mixed_S[i].Accident = 1;
                                }
                                
                                //Store in Wallsten Measures for Each Frame
                                Vehicles[n].VehFrames[f].WallstenAcceleration = At_Update;
                                Vehicles[n].VehFrames[f].WallstenVelocity = Vt_Update;
                                Vehicles[n].VehFrames[f].WallstenX = Xt_Update;
                                Vehicles[n].VehFrames[f].WallstenRelativeSpeed = DeltaV_Update;
                                Vehicles[n].VehFrames[f].WallstenSpaceHeadway = DeltaX_Update;
                                Vehicles[n].VehFrames[f].WallstenGap = S_Update;//Possible Error: Problem Since Space Gaps are Sometimes Negative: Use Space Headway Instead
                                
                                FrameTime[x_c].FrameLocation[y_c].WallstenVelocity = Vehicles[n].VehFrames[f].WallstenVelocity;
                                FrameTime[x_c].FrameLocation[y_c].WallstenX = Vehicles[n].VehFrames[f].WallstenX;
                                FrameTime[x_c].FrameLocation[y_c].LeadersChangingLanes_Frame = Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame;
                                
                            }
                            else
                            {
                                //Modified by Samer Hamdar: 8 October 2008
                                
                                //Changed Lanes: New Episode
                                if (Vehicles[n].VehFrames[f].Lane != Lane_Previous)
                                {
                                    RT_Counter = 0;
                                    
                                    At_Update = Vehicles[n].VehFrames[f].VehicleAcceleration;
                                    Vt_Update = Vehicles[n].VehFrames[f].VehicleVelocity;
                                    Xt_Update = Vehicles[n].VehFrames[f].LocalY;
                                    DeltaX_Update = Vehicles[n].VehFrames[f].SpaceHeadway;
                                    DeltaV_Update = Vehicles[n].VehFrames[f].RelativeVelocity;
                                    S_Update = Vehicles[n].VehFrames[f].SpaceGap;
                                    
                                    //New Episode:
                                    if (f != 0)//Not First Reading
                                    {
                                        Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame = 0;
                                        Vehicles[n].VehFrames[f].FirstEpisodeDummy_Frame = 0;
                                        Vehicles[n].VehFrames[f].DurationTime_Frame = 0;
                                    }
                                    
                                    //Store Lane Changing Hazard Struct
                                    Vehicles[n].NumberOfLaneChanges = Vehicles[n].NumberOfLaneChanges + 1;
                                    LaneChangingData lane_changing_data;
                                    lane_changing_data.LaneChangingTime = Vehicles[n].VehFrames[f].FrameID;
                                    
                                    if (Vehicles[n].VehFrames[f].Mode == 1 )
                                    {
                                        lane_changing_data.LaneChangingHazard = Vehicles[n].VehFrames[f - 1].LaneChange_Hazard_CF;
                                        lane_changing_data.CompetingHazard = Vehicles[n].VehFrames[f - 1].Mode_Hazard_CF;
                                        lane_changing_data.Mode = Vehicles[n].VehFrames[f].Mode;
                                    }
                                    else if (Vehicles[n].VehFrames[f].Mode == 0)
                                    {
                                        lane_changing_data.LaneChangingHazard = Vehicles[n].VehFrames[f - 1].LaneChange_Hazard_FF;
                                        lane_changing_data.CompetingHazard = Vehicles[n].VehFrames[f - 1].Mode_Hazard_FF;
                                        lane_changing_data.Mode = Vehicles[n].VehFrames[f].Mode;
                                    }
                                    
                                    Vehicles[n].LaneChanging.push_back(lane_changing_data);
                                }
                                else //Did not Change Lanes
                                {
                                    Random_Wiener = ( (rand()%1000) / 1000.0 );
                                    Yt_previous = Yt;
                                    Yt = Yt * exp ( - DeltaT / Tau) + sqrt ( 24.0 * DeltaT / Tau ) * Random_Wiener;
                                    //At_Update = Astar + Var * Yt;
                                    At_Update = At_Update;
                                    
                                    Vt_Update = Vt_Update + ( At_Update * DeltaT );
                                    
                                    if ( Vt_Update <= 0.0 )//Problem: Test: To Avoid Negative Velocities (?)
                                        Vt_Update = 0.1;
                                    
                                    Xt_Update = Xt_Update + ( Vt_Update * DeltaT ) + 0.5 * ( At_Update * DeltaT * DeltaT );
                                    DeltaX_Update = Vehicles[n].VehFrames[f].SpaceHeadway + ( Vehicles[n].VehFrames[f].LocalY - Xt_Update );
                                    DeltaV_Update = Vehicles[n].VehFrames[f].RelativeVelocity + ( Vt_Update - Vehicles[n].VehFrames[f].VehicleVelocity );
                                    S_Update = Vehicles[n].VehFrames[f].SpaceGap + ( Vehicles[n].VehFrames[f].LocalY - Xt_Update );//Possible Error: Problem Since Space Gaps are Sometimes Negative: Use Space Headway Instead
                                    
                                    //To Detect Accidents
                                    if (S_Update <= 0)
                                    {
                                        S_Update = 0.1;
                                        Vt_Update = 0.1;
                                        At_Update = 0.0;
                                        Vehicles[n].VehParameterChildren_Mixed_S[i].Accident = 1;
                                    }
                                    
                                    if (Vehicles[n].VehFrames[f].PreVehID != Leader_Previous)
                                    {
                                        if (f != 0)//Not First Reading
                                            Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame = Vehicles[n].VehFrames[f-1].LeadersChangingLanes_Frame + 1;
                                    }
                                }
                                
                                //_______________________________Added by Samer Hamdar: 13 October 2008
                                
                                //Compute Variables for Hazard Computation
                                
                                AverageSurroundingDensitytmp = 0.0;
                                AverageSurroundingDensityLaneRtmp = 0.0;
                                AverageSurroundingDensityLaneLtmp = 0.0;
                                
                                x = 0;//to track frametime data
                                y = 0;//to track frametime.framelocation data
                                
                                while (x < t) //Search for the Frame Limiting The Episode in the Sorted Data in Time Level
                                {
                                    y = 0;// Initialize y to go through FrameLocations Corresponding to Frame Time Slot x
                                    
                                    while (y < FrameTime[x].NumFramesLocation)//Search for the Frame Limiting The Episode in the Sorted Data in Space Level
                                    {
                                        if (FrameTime[x].FrameLocation[y].FrameGlobalID == Vehicles[n].VehFrames[f].FrameGlobalID)
                                        {
                                            
                                            x_c = x;
                                            y_c = y;
                                            //Speed
                                            Vehicles[n].VehFrames[f].DriverSpeed_Frame = Vt_Update;
                                            
                                            //Distance_To_Ramp
                                            Vehicles[n].VehFrames[f].DistanceToRamp_Frame = fabs ( RampLength - Xt_Update);
                                            
                                            
                                            //First Leader Related Data (Same Lane)
                                            sw = FrameTime[x].NumFramesLocation - y;
                                            
                                            dummy = 0;
                                            for (int w = 1; w < sw; w++)
                                            {
                                                if (FrameTime[x].FrameLocation[y].Lane == FrameTime[x].FrameLocation[y+w].Lane)
                                                {
                                                    dummy = 1;
                                                    Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame = (Vt_Update - FrameTime[x].FrameLocation[y+w].VehicleVelocity);
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayLeader1_Frame = (FrameTime[x].FrameLocation[y+w].LocalY - Xt_Update);
                                                    w = sw+1;
                                                }
                                            }
                                            if (dummy == 0)//No First Leader
                                            {
                                                if ( FrameTime[x].FrameLocation[y].Lane == 7 )
                                                {
                                                    Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame = Vt_Update;
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayLeader1_Frame = ( RampLength - Xt_Update);
                                                }
                                                else
                                                {
                                                    Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame = -1.1234;
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayLeader1_Frame = 20.1234;
                                                    
                                                    Vehicles[n].VehFrames[f].Flag1 = 1;
                                                }
                                            }
                                            
                                            //Second Leader Related Data (Same Lane)
                                            dummy = 0;
                                            track = 2;
                                            for (int w = 1; w < sw; w++)
                                            {
                                                if (FrameTime[x].FrameLocation[y].Lane == FrameTime[x].FrameLocation[y+w].Lane)
                                                {
                                                    track --;
                                                    if (track == 0)
                                                    {
                                                        dummy = 1;
                                                        Vehicles[n].VehFrames[f].RelativeSpeedLeader2_Frame = ( Vt_Update - FrameTime[x].FrameLocation[y+w].VehicleVelocity);
                                                        Vehicles[n].VehFrames[f].DistanceHeadwayLeader2_Frame = (FrameTime[x].FrameLocation[y+w].LocalY - Xt_Update);
                                                        w = sw+1;
                                                    }
                                                }
                                            }
                                            if (dummy == 0)//No Second Leader
                                            {
                                                Vehicles[n].VehFrames[f].RelativeSpeedLeader2_Frame = -1.1234;
                                                Vehicles[n].VehFrames[f].DistanceHeadwayLeader2_Frame = 35.1234;
                                                
                                                Vehicles[n].VehFrames[f].Flag1 = 1;
                                            }
                                            
                                            //First Follower Related Data (Same Lane)
                                            dummy = 0;
                                            for (int w = 1; w <= y; w++)
                                            {
                                                if (FrameTime[x].FrameLocation[y].Lane == FrameTime[x].FrameLocation[y-w].Lane)
                                                {
                                                    dummy = 1;
                                                    Vehicles[n].VehFrames[f].RelativeSpeedFollower1_Frame = (Vt_Update - FrameTime[x].FrameLocation[y-w].VehicleVelocity);
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayFollower1_Frame = (Xt_Update - FrameTime[x].FrameLocation[y-w].LocalY);
                                                    w = y+1;
                                                }
                                            }
                                            if (dummy == 0)//No First Follower
                                            {
                                                Vehicles[n].VehFrames[f].RelativeSpeedFollower1_Frame = 1.1234;
                                                Vehicles[n].VehFrames[f].DistanceHeadwayFollower1_Frame = 20.1234;
                                                
                                                Vehicles[n].VehFrames[f].Flag1 = 1;
                                            }
                                            
                                            //First Leader Related Data (Left Lane)
                                            sw = FrameTime[x].NumFramesLocation - y;
                                            dummy = 0;
                                            for (int w = 1; w < sw; w++)
                                            {
                                                if (FrameTime[x].FrameLocation[y].Lane == (FrameTime[x].FrameLocation[y+w].Lane + 1))
                                                {
                                                    dummy = 1;
                                                    Vehicles[n].VehFrames[f].RelativeSpeedLeftL1_Frame = ( Vt_Update - FrameTime[x].FrameLocation[y+w].VehicleVelocity);
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayLeftL1_Frame = (FrameTime[x].FrameLocation[y+w].LocalY - Xt_Update);
                                                    w = sw+1;
                                                }
                                            }
                                            if (dummy == 0)//No First Leader / (Lane 1) (NB: Data related to Lane 9 - shoulder are ignored) + (Lane Seven Always Consider Headway Ahead)
                                            {
                                                //veh_duration_data_SUB.RelativeSpeedLeftL1 = 999;
                                                //veh_duration_data_SUB.DistanceHeadwayLeftL1 = 9999;
                                                if ( FrameTime[x].FrameLocation[y].Lane == 1 )
                                                {
                                                    Vehicles[n].VehFrames[f].RelativeSpeedLeftL1_Frame = 30.1234;
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayLeftL1_Frame = 0.1234;
                                                }
                                                else
                                                {
                                                    Vehicles[n].VehFrames[f].RelativeSpeedLeftL1_Frame = -1.1234;
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayLeftL1_Frame = 20.1234;
                                                    
                                                    Vehicles[n].VehFrames[f].Flag1 = 1;
                                                }
                                            }
                                            
                                            //First Leader Related Data (Right Lane)
                                            dummy = 0;
                                            for (int w = 1; w < sw; w++)
                                            {
                                                if (FrameTime[x].FrameLocation[y].Lane == (FrameTime[x].FrameLocation[y+w].Lane - 1) && FrameTime[x].FrameLocation[y].Lane != 6)
                                                {
                                                    dummy = 1;
                                                    Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame = (Vt_Update - FrameTime[x].FrameLocation[y+w].VehicleVelocity);
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame = (FrameTime[x].FrameLocation[y+w].LocalY - Xt_Update);
                                                    w = sw+1;
                                                }
                                                else if (  FrameTime[x].FrameLocation[y].Lane == 6  )
                                                {
                                                    if ( FrameTime[x].FrameLocation[y].LocalY > 91 && FrameTime[x].FrameLocation[y].LocalY < 215)
                                                    {
                                                        dummy = 1;
                                                        Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame = (Vt_Update - FrameTime[x].FrameLocation[y+w].VehicleVelocity);
                                                        Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame = (FrameTime[x].FrameLocation[y+w].LocalY - Xt_Update);
                                                        w = sw+1;
                                                        
                                                        Vehicles[n].VehFrames[f].Flag2 = 1;
                                                    }
                                                    else
                                                    {
                                                        dummy = 1;
                                                        Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame = 30.1234;
                                                        Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame = 0.1234;
                                                    }
                                                    
                                                }
                                            }
                                            if (dummy == 0)//No First Leader / Lane 7/ (NB: Data related to Lane 9 - shoulder are ignored)
                                            {
                                                //veh_duration_data_SUB.RelativeSpeedRightL1 = 999;
                                                //veh_duration_data_SUB.DistanceHeadwayRightL1 = 9999;
                                                if ( FrameTime[x].FrameLocation[y].Lane == 7 )
                                                {
                                                    Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame = 30.1234;
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame = 0.1234;
                                                }
                                                else
                                                {
                                                    Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame = -1.1234;
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame = 20.1234;
                                                    
                                                    Vehicles[n].VehFrames[f].Flag1 = 1;
                                                }
                                            }
                                            
                                            
                                            //First Follower Related Data (Left Lane)
                                            dummy = 0;
                                            for (int w = 1; w <= y; w++)
                                            {
                                                if (FrameTime[x].FrameLocation[y].Lane == (FrameTime[x].FrameLocation[y-w].Lane + 1))
                                                {
                                                    dummy = 1;
                                                    Vehicles[n].VehFrames[f].RelativeSpeedLeftF1_Frame =(Vt_Update - FrameTime[x].FrameLocation[y-w].VehicleVelocity);
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayLeftF1_Frame = (Xt_Update - FrameTime[x].FrameLocation[y-w].LocalY);
                                                    w = y+1;
                                                }
                                            }
                                            if (dummy == 0)//No First Follower / (Lane 1)
                                            {
                                                //veh_duration_data_SUB.RelativeSpeedLeftF1 = 999;
                                                //veh_duration_data_SUB.DistanceHeadwayLeftF1 = 9999;
                                                if ( FrameTime[x].FrameLocation[y].Lane == 1 )
                                                {
                                                    Vehicles[n].VehFrames[f].RelativeSpeedLeftF1_Frame = -30.1234;
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayLeftF1_Frame = 0.1234;
                                                }
                                                else
                                                {
                                                    Vehicles[n].VehFrames[f].RelativeSpeedLeftF1_Frame = 1.1234;
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayLeftF1_Frame = 20.1234;
                                                    
                                                    Vehicles[n].VehFrames[f].Flag1 = 1;
                                                }
                                            }
                                            
                                            //First Follower Related Data (Right Lane)
                                            dummy = 0;
                                            for (int w = 1; w <= y; w++)
                                            {
                                                if (FrameTime[x].FrameLocation[y].Lane == (FrameTime[x].FrameLocation[y-w].Lane - 1) && FrameTime[x].FrameLocation[y].Lane != 6)
                                                {
                                                    dummy = 1;
                                                    Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame = (Vt_Update - FrameTime[x].FrameLocation[y-w].VehicleVelocity);
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame = (Xt_Update - FrameTime[x].FrameLocation[y-w].LocalY);
                                                    w = y+1;
                                                }
                                                else if ( FrameTime[x].FrameLocation[y].Lane == 6 )
                                                {
                                                    if ( FrameTime[x].FrameLocation[y].LocalY > 91 && FrameTime[x].FrameLocation[y].LocalY < 215)
                                                    {
                                                        dummy = 1;
                                                        Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame = (Vt_Update - FrameTime[x].FrameLocation[y-w].VehicleVelocity);
                                                        Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame = (Xt_Update - FrameTime[x].FrameLocation[y-w].LocalY);
                                                        w = y+1;
                                                        
                                                        Vehicles[n].VehFrames[f].Flag2 = 1;
                                                    }
                                                    else
                                                    {
                                                        dummy = 1;
                                                        Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame = -30.1234;
                                                        Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame = 0.1234;
                                                    }
                                                }
                                            }
                                            if (dummy == 0)//No First Follower / (Lane 7)
                                            {
                                                //veh_duration_data_SUB.RelativeSpeedRightF1 = 999;
                                                //veh_duration_data_SUB.DistanceHeadwayRightF1 = 9999;
                                                if ( FrameTime[x].FrameLocation[y].Lane == 7 )
                                                {
                                                    Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame = - 30.1234;
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame = 0.1234;
                                                }
                                                else
                                                {
                                                    Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame = 1.1234;
                                                    Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame = 20.1234;
                                                    
                                                    Vehicles[n].VehFrames[f].Flag1 = 1;
                                                }
                                            }
                                            
                                            //Average Density Related Data
                                            AverageSurroundingDensitytmp = 1;//Modified by Samer Hamdar, April 5, 2007: the vehicle of interest is considered in the surrounding density
                                            AverageSurroundingDensityLaneRtmp = 0;
                                            AverageSurroundingDensityLaneLtmp = 0;
                                            
                                            //General
                                            ww = 1;
                                            //Ahead
                                            while ( ww < sw)
                                            {
                                                if ( FrameTime[x].FrameLocation[y + ww].LocalY - Xt_Update <= DistanceDensityAhead)
                                                {
                                                    AverageSurroundingDensitytmp = AverageSurroundingDensitytmp + 1;
                                                }
                                                ww = ww + 1;
                                            }
                                            
                                            //Behind
                                            ww = 1;
                                            while (ww <= y)
                                            {
                                                if (  Xt_Update  - FrameTime[x].FrameLocation[y - ww].LocalY <= DistanceDensityBehind)
                                                {
                                                    AverageSurroundingDensitytmp = AverageSurroundingDensitytmp + 1;
                                                }
                                                ww = ww + 1;
                                            }
                                            
                                            //Interpolation
                                            if ((Xt_Update < DistanceDensityBehind) && (L - Xt_Update >= DistanceDensityAhead))
                                            {
                                                AverageSurroundingDensitytmp = (AverageSurroundingDensitytmp * (DistanceDensityAhead + DistanceDensityBehind) / (Xt_Update + DistanceDensityAhead));
                                            }
                                            
                                            if ((Xt_Update >= DistanceDensityBehind) && (L - Xt_Update < DistanceDensityAhead))
                                            {
                                                AverageSurroundingDensitytmp = (AverageSurroundingDensitytmp * (DistanceDensityAhead + DistanceDensityBehind) / (DistanceDensityBehind + L - Xt_Update));
                                            }
                                            
                                            if ((Xt_Update < DistanceDensityBehind) && (L - Xt_Update < DistanceDensityAhead))
                                            {
                                                AverageSurroundingDensitytmp = (AverageSurroundingDensitytmp * (DistanceDensityAhead + DistanceDensityBehind) / L);
                                            }
                                            
                                            AverageSurroundingDensitytmp = ((AverageSurroundingDensitytmp * 1000.0)/(DistanceDensityAhead + DistanceDensityBehind) / NLanes);
                                            Vehicles[n].VehFrames[f].AverageSurroundingDensity_Frame = AverageSurroundingDensitytmp;
                                            
                                            
                                            //Average Density to the Right Related Data
                                            ww = 1;
                                            //Ahead
                                            while ( ww < sw)
                                            {
                                                if ((FrameTime[x].FrameLocation[y + ww].LocalY - Xt_Update <= DistanceDensityAhead) && (FrameTime[x].FrameLocation[y].Lane == (FrameTime[x].FrameLocation[y + ww].Lane - 1) ) )
                                                {
                                                    AverageSurroundingDensityLaneRtmp = AverageSurroundingDensityLaneRtmp + 1;
                                                }
                                                ww = ww + 1;
                                            }
                                            
                                            //Behind
                                            ww = 1;
                                            while (ww <= y)
                                            {
                                                if (  Xt_Update  - FrameTime[x].FrameLocation[y - ww].LocalY <= DistanceDensityBehind && (FrameTime[x].FrameLocation[y].Lane == (FrameTime[x].FrameLocation[y-ww].Lane - 1)))
                                                {
                                                    AverageSurroundingDensityLaneRtmp = AverageSurroundingDensityLaneRtmp + 1;
                                                }
                                                ww = ww + 1;
                                            }
                                            
                                            //Interpolation
                                            if ((Xt_Update < DistanceDensityBehind) && (L - Xt_Update >= DistanceDensityAhead))
                                            {
                                                AverageSurroundingDensityLaneRtmp = (AverageSurroundingDensityLaneRtmp * (DistanceDensityAhead + DistanceDensityBehind) / (Xt_Update + DistanceDensityAhead));
                                            }
                                            
                                            if ((Xt_Update >= DistanceDensityBehind) && (L - Xt_Update < DistanceDensityAhead))
                                            {
                                                AverageSurroundingDensityLaneRtmp = (AverageSurroundingDensityLaneRtmp * (DistanceDensityAhead + DistanceDensityBehind) / (DistanceDensityBehind + L - Xt_Update));
                                            }
                                            
                                            if ((Xt_Update < DistanceDensityBehind) && (L - Xt_Update < DistanceDensityAhead))
                                            {
                                                AverageSurroundingDensityLaneRtmp = (AverageSurroundingDensityLaneRtmp * (DistanceDensityAhead + DistanceDensityBehind) / L);
                                            }
                                            
                                            AverageSurroundingDensityLaneRtmp = ((AverageSurroundingDensityLaneRtmp * 1000.0)/(DistanceDensityAhead + DistanceDensityBehind));// 1 mile = 5280 fts
                                            
                                            
                                            if (FrameTime[x].FrameLocation[y].Lane == 6)
                                            {
                                                if ( Xt_Update > 91 && Xt_Update < 215)
                                                {
                                                    AverageSurroundingDensityLaneRtmp = AverageSurroundingDensityLaneRtmp;
                                                }
                                                else
                                                {
                                                    AverageSurroundingDensityLaneRtmp = 100.1234;
                                                    
                                                }
                                            }
                                            
                                            if (FrameTime[x].FrameLocation[y].Lane == 7)
                                            {
                                                AverageSurroundingDensityLaneRtmp = 100.1234;
                                            }
                                            
                                            Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneR_Frame = AverageSurroundingDensityLaneRtmp;
                                            
                                            //Average Density to the Left Related Data
                                            ww = 1;
                                            //Ahead
                                            while ( ww < sw)
                                            {
                                                if ((FrameTime[x].FrameLocation[y + ww].LocalY - Xt_Update <= DistanceDensityAhead) && (FrameTime[x].FrameLocation[y].Lane == (FrameTime[x].FrameLocation[y + ww].Lane + 1)) )
                                                {
                                                    AverageSurroundingDensityLaneLtmp = AverageSurroundingDensityLaneLtmp + 1;
                                                }
                                                ww = ww + 1;
                                            }
                                            
                                            //Behind
                                            ww = 1;
                                            while (ww <= y)
                                            {
                                                if (  ( Xt_Update  - FrameTime[x].FrameLocation[y-ww].LocalY <= DistanceDensityBehind ) && ( FrameTime[x].FrameLocation[y].Lane == (FrameTime[x].FrameLocation[y-ww].Lane + 1) ) )
                                                {
                                                    AverageSurroundingDensityLaneLtmp = AverageSurroundingDensityLaneLtmp + 1;
                                                }
                                                ww = ww + 1;
                                            }
                                            
                                            //Interpolation
                                            if ((Xt_Update < DistanceDensityBehind) && (L - Xt_Update >= DistanceDensityAhead))
                                            {
                                                AverageSurroundingDensityLaneLtmp = (AverageSurroundingDensityLaneLtmp * (DistanceDensityAhead + DistanceDensityBehind) / (Xt_Update + DistanceDensityAhead));
                                            }
                                            
                                            if ((Xt_Update >= DistanceDensityBehind) && (L - Xt_Update < DistanceDensityAhead))
                                            {
                                                AverageSurroundingDensityLaneLtmp = (AverageSurroundingDensityLaneLtmp * (DistanceDensityAhead + DistanceDensityBehind) / (DistanceDensityBehind + L - Xt_Update));
                                            }
                                            
                                            if ((Xt_Update < DistanceDensityBehind) && (L - Xt_Update < DistanceDensityAhead))
                                            {
                                                AverageSurroundingDensityLaneLtmp = (AverageSurroundingDensityLaneLtmp * (DistanceDensityAhead + DistanceDensityBehind) / L);
                                                
                                            }
                                            
                                            AverageSurroundingDensityLaneLtmp = ((AverageSurroundingDensityLaneLtmp * 1000.0)/(DistanceDensityAhead + DistanceDensityBehind));// 1 mile = 5280 fts
                                            
                                            if (FrameTime[x].FrameLocation[y].Lane == 1)
                                            {
                                                AverageSurroundingDensityLaneLtmp = 100.1234;
                                            }
                                            
                                            Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneL_Frame = AverageSurroundingDensityLaneLtmp;
                                            
                                            break;
                                        }
                                        y++;
                                    }
                                    x++;
                                }
                                
                                //Hazard: Car-Following wih Lane Change Exit
                                
                                //__Assuming Weibull Distribution
                                
                                //____Number One: Limdep Technique
                                Lambda_tmp = exp ( -1.0 * ( BetaOne_CF_LC + BetaLCL_CF_LC * Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame + BetaV_CF_LC * Vehicles[n].VehFrames[f].DriverSpeed_Frame + BetaDXL1_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeader1_Frame + BetaDVL1_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame + BetaDXL2_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeader2_Frame + BetaDVL2_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeader2_Frame + BetaDXF1_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayFollower1_Frame + BetaDVF1_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedFollower1_Frame + BetaDXL1R_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame + BetaDVL1R_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame + BetaDXF1R_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame + BetaDVF1R_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame + BetaDXL1L_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeftL1_Frame + BetaDVL1L_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeftL1_Frame + BetaDXF1L_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeftF1_Frame + BetaDVF1L_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeftF1_Frame + BetaKR_CF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneR_Frame + BetaKL_CF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneL_Frame + BetaK_CF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensity_Frame + BetaRightLane_CF_LC * Vehicles[n].VehFrames[f].RightLaneDummy_Frame + BetaLeftLane_CF_LC * Vehicles[n].VehFrames[f].LeftLaneDummy_Frame + BetaRamp_CF_LC * Vehicles[n].VehFrames[f].RampDummy_Frame + BetaDistanceToRamp_CF_LC * Vehicles[n].VehFrames[f].DistanceToRamp_Frame) );
                                
                                Hazard_ParameterA_CF_LC = Lambda_tmp;
                                Hazard_tmp = Hazard_ParameterA_CF_LC * Hazard_ParameterB_CF_LC * pow ( ( Hazard_ParameterA_CF_LC * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ),( Hazard_ParameterB_CF_LC - 1 ) );
                                Survival_tmp = exp (  -1.0 * pow ( ( Hazard_ParameterA_CF_LC * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ) , Hazard_ParameterB_CF_LC )  );
                                
                                R_Heterogeneity_CF_LC = K_Heterogeneity_CF_LC;
                                
                                //Teta_tmp = ( 1.0 / K_Heterogeneity_CF_LC );
                                Teta_tmp = 0.55243252;
                                
                                Survival2_tmp = Survival_tmp * Teta_tmp;
                                
                                Hazard2_tmp = Survival2_tmp * Hazard_tmp;
                                
                                //____Number Two: Bhat Technique
                                //R_Heterogeneity_CF_LC = K_Heterogeneity_CF_LC;
                                //Hazard_tmp = Hazard_ParameterA_CF_LC * Hazard_ParameterB_CF_LC * pow ( ( Hazard_ParameterA_CF_LC * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ),( Hazard_ParameterB_CF_LC - 1 ) );
                                //Survival_tmp = exp (  -1.0 * pow ( ( Hazard_ParameterA_CF_LC * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ) , Hazard_ParameterB_CF_LC )  );
                                //
                                ////Wrong:Gamma_tmp = ( pow ( K_Heterogeneity_CF_LC, R_Heterogeneity_CF_LC ) / double ( factorial ( int ( R_Heterogeneity_CF_LC - 1 ) ) ) ) * exp ( - 1.0 * K_Heterogeneity_CF_LC * V_tmp) * pow ( V_tmp , ( R_Heterogeneity_CF_LC - 1)  );
                                //
                                //Gamma_tmp = Vehicles[n].VehFrames[f].GammaHeterogeneity_CF_LC;
                                //W_tmp = log (Gamma_tmp);//(?): What is
                                //Lambda_tmp = exp ( -1.0 * ( BetaOne_CF_LC + BetaLCL_CF_LC * Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame + BetaV_CF_LC * Vehicles[n].VehFrames[f].DriverSpeed_Frame + BetaDXL1_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeader1_Frame + BetaDVL1_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame + BetaDXL2_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeader2_Frame + BetaDVL2_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeader2_Frame + BetaDXF1_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayFollower1_Frame + BetaDVF1_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedFollower1_Frame + BetaDXL1R_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame + BetaDVL1R_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame + BetaDXF1R_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame + BetaDVF1R_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame + BetaDXL1L_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeftL1_Frame + BetaDVL1L_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeftL1_Frame + BetaDXF1L_CF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeftF1_Frame + BetaDVF1L_CF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeftF1_Frame + BetaKR_CF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneR_Frame + BetaKL_CF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneL_Frame + BetaK_CF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensity_Frame + BetaRightLane_CF_LC * Vehicles[n].VehFrames[f].RightLaneDummy_Frame + BetaLeftLane_CF_LC * Vehicles[n].VehFrames[f].LeftLaneDummy_Frame + BetaRamp_CF_LC * Vehicles[n].VehFrames[f].RampDummy_Frame + BetaDistanceToRamp_CF_LC * Vehicles[n].VehFrames[f].DistanceToRamp_Frame)+ W_tmp );
                                //Hazard2_tmp = Hazard_tmp * Lambda_tmp;
                                
                                Vehicles[n].VehFrames[f].LaneChange_Hazard_CF = Hazard2_tmp;
                                
                                //Hazard: Car-Following with Mode Exit
                                
                                //__Assuming Weibull Distribution
                                
                                //____Number One: Limdep Technique
                                Lambda_tmp = exp ( -1.0 * ( BetaOne_CF_CM + BetaLCL_CF_CM * Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame + BetaV_CF_CM * Vehicles[n].VehFrames[f].DriverSpeed_Frame + BetaDXL1_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeader1_Frame + BetaDVL1_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame + BetaDXL2_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeader2_Frame + BetaDVL2_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeader2_Frame + BetaDXF1_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayFollower1_Frame + BetaDVF1_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedFollower1_Frame + BetaDXL1R_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame + BetaDVL1R_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame + BetaDXF1R_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame + BetaDVF1R_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame + BetaDXL1L_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeftL1_Frame + BetaDVL1L_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeftL1_Frame + BetaDXF1L_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeftF1_Frame + BetaDVF1L_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeftF1_Frame + BetaKR_CF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneR_Frame + BetaKL_CF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneL_Frame + BetaK_CF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensity_Frame + BetaRightLane_CF_CM * Vehicles[n].VehFrames[f].RightLaneDummy_Frame + BetaLeftLane_CF_CM * Vehicles[n].VehFrames[f].LeftLaneDummy_Frame + BetaRamp_CF_CM * Vehicles[n].VehFrames[f].RampDummy_Frame + BetaDistanceToRamp_CF_CM * Vehicles[n].VehFrames[f].DistanceToRamp_Frame) );
                                
                                Hazard_ParameterA_CF_CM = Lambda_tmp;
                                Hazard_tmp = Hazard_ParameterA_CF_CM * Hazard_ParameterB_CF_CM * pow ( ( Hazard_ParameterA_CF_CM * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ),( Hazard_ParameterB_CF_CM - 1 ) );
                                Survival_tmp = exp (  -1.0 * pow ( ( Hazard_ParameterA_CF_CM * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ) , Hazard_ParameterB_CF_CM )  );
                                
                                R_Heterogeneity_CF_CM = K_Heterogeneity_CF_CM;
                                
                                //Teta_tmp = ( 1.0 / K_Heterogeneity_CF_CM );
                                Teta_tmp = 0.55243252;
                                
                                Survival2_tmp = Survival_tmp * Teta_tmp;
                                
                                Hazard2_tmp = Survival2_tmp * Hazard_tmp;
                                
                                //____Number Two: Bhat Technique
                                //R_Heterogeneity_CF_CM = K_Heterogeneity_CF_CM;
                                //Hazard_tmp = Hazard_ParameterA_CF_CM * Hazard_ParameterB_CF_CM * pow ( ( Hazard_ParameterA_CF_CM * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ),( Hazard_ParameterB_CF_CM - 1 ) );
                                //Survival_tmp = exp (  -1.0 * pow ( ( Hazard_ParameterA_CF_CM * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ) , Hazard_ParameterB_CF_CM )  );
                                //
                                ////Wrong:Gamma_tmp = ( pow ( K_Heterogeneity_CF_CM, R_Heterogeneity_CF_CM ) / double ( factorial ( int ( R_Heterogeneity_CF_CM - 1 ) ) ) ) * exp ( - 1.0 * K_Heterogeneity_CF_CM * V_tmp) * pow ( V_tmp , ( R_Heterogeneity_CF_CM - 1)  );
                                //
                                //Gamma_tmp = Vehicles[n].VehFrames[f].GammaHeterogeneity_CF_CM;
                                //W_tmp = log (Gamma_tmp);//(?): What is
                                //Lambda_tmp = exp ( -1.0 * ( BetaOne_CF_CM + BetaLCL_CF_CM * Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame + BetaV_CF_CM * Vehicles[n].VehFrames[f].DriverSpeed_Frame + BetaDXL1_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeader1_Frame + BetaDVL1_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame + BetaDXL2_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeader2_Frame + BetaDVL2_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeader2_Frame + BetaDXF1_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayFollower1_Frame + BetaDVF1_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedFollower1_Frame + BetaDXL1R_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame + BetaDVL1R_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame + BetaDXF1R_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame + BetaDVF1R_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame + BetaDXL1L_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeftL1_Frame + BetaDVL1L_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeftL1_Frame + BetaDXF1L_CF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeftF1_Frame + BetaDVF1L_CF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeftF1_Frame + BetaKR_CF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneR_Frame + BetaKL_CF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneL_Frame + BetaK_CF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensity_Frame + BetaRightLane_CF_CM * Vehicles[n].VehFrames[f].RightLaneDummy_Frame + BetaLeftLane_CF_CM * Vehicles[n].VehFrames[f].LeftLaneDummy_Frame + BetaRamp_CF_CM * Vehicles[n].VehFrames[f].RampDummy_Frame + BetaDistanceToRamp_CF_CM * Vehicles[n].VehFrames[f].DistanceToRamp_Frame)+ W_tmp );
                                //Hazard2_tmp = Hazard_tmp * Lambda_tmp;
                                
                                Vehicles[n].VehFrames[f].Mode_Hazard_CF = Hazard2_tmp;
                                
                                //Hazard: Free Flow with Lane Change Exit
                                
                                //__Assuming Exponential Distribution
                                
                                //____Number One: Limdep Technique
                                Lambda_tmp = exp ( -1.0 * ( BetaOne_FF_LC + BetaLCL_FF_LC * Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame + BetaV_FF_LC * Vehicles[n].VehFrames[f].DriverSpeed_Frame + BetaDXL1_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeader1_Frame + BetaDVL1_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame + BetaDXL2_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeader2_Frame + BetaDVL2_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeader2_Frame + BetaDXF1_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayFollower1_Frame + BetaDVF1_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedFollower1_Frame + BetaDXL1R_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame + BetaDVL1R_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame + BetaDXF1R_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame + BetaDVF1R_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame + BetaDXL1L_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeftL1_Frame + BetaDVL1L_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeftL1_Frame + BetaDXF1L_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeftF1_Frame + BetaDVF1L_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeftF1_Frame + BetaKR_FF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneR_Frame + BetaKL_FF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneL_Frame + BetaK_FF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensity_Frame + BetaRightLane_FF_LC * Vehicles[n].VehFrames[f].RightLaneDummy_Frame + BetaLeftLane_FF_LC * Vehicles[n].VehFrames[f].LeftLaneDummy_Frame + BetaRamp_FF_LC * Vehicles[n].VehFrames[f].RampDummy_Frame + BetaDistanceToRamp_FF_LC * Vehicles[n].VehFrames[f].DistanceToRamp_Frame) );
                                
                                Hazard_ParameterA_FF_LC = Lambda_tmp;
                                Hazard_ParameterB_FF_LC = 1.0;//Difference Between Exponential and Weibull
                                Hazard_tmp = Hazard_ParameterA_FF_LC * Hazard_ParameterB_FF_LC * pow ( ( Hazard_ParameterA_FF_LC * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ),( Hazard_ParameterB_FF_LC - 1 ) );
                                Survival_tmp = exp (  -1.0 * pow ( ( Hazard_ParameterA_FF_LC * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ) , Hazard_ParameterB_FF_LC )  );
                                
                                R_Heterogeneity_FF_LC = K_Heterogeneity_FF_LC;
                                
                                //Teta_tmp = ( 1.0 / K_Heterogeneity_FF_LC );
                                Teta_tmp = 0.55243252;
                                
                                Survival2_tmp = Survival_tmp * Teta_tmp;
                                
                                Hazard2_tmp = Survival2_tmp * Hazard_tmp;
                                
                                //____Number Two: Bhat Technique
                                //R_Heterogeneity_FF_LC = K_Heterogeneity_FF_LC;
                                //Hazard_ParameterB_FF_LC = 1.0;
                                //Hazard_tmp = Hazard_ParameterA_FF_LC * Hazard_ParameterB_FF_LC * pow ( ( Hazard_ParameterA_FF_LC * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ),( Hazard_ParameterB_FF_LC - 1 ) );
                                //Survival_tmp = exp (  -1.0 * pow ( ( Hazard_ParameterA_FF_LC * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ) , Hazard_ParameterB_FF_LC )  );
                                //
                                ////Wrong:Gamma_tmp = ( pow ( K_Heterogeneity_FF_LC, R_Heterogeneity_FF_LC ) / double ( factorial ( int ( R_Heterogeneity_FF_LC - 1 ) ) ) ) * exp ( - 1.0 * K_Heterogeneity_FF_LC * V_tmp) * pow ( V_tmp , ( R_Heterogeneity_FF_LC - 1)  );
                                //
                                ////Gamma_tmp = Vehicles[n].VehFrames[f].GammaHeterogeneity_FF_LC;
                                //W_tmp = log (Gamma_tmp);//(?): What is v
                                //Lambda_tmp = exp ( -1.0 * ( BetaOne_FF_LC + BetaLCL_FF_LC * Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame + BetaV_FF_LC * Vehicles[n].VehFrames[f].DriverSpeed_Frame + BetaDXL1_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeader1_Frame + BetaDVL1_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame + BetaDXL2_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeader2_Frame + BetaDVL2_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeader2_Frame + BetaDXF1_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayFollower1_Frame + BetaDVF1_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedFollower1_Frame + BetaDXL1R_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame + BetaDVL1R_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame + BetaDXF1R_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame + BetaDVF1R_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame + BetaDXL1L_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeftL1_Frame + BetaDVL1L_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeftL1_Frame + BetaDXF1L_FF_LC * Vehicles[n].VehFrames[f].DistanceHeadwayLeftF1_Frame + BetaDVF1L_FF_LC * Vehicles[n].VehFrames[f].RelativeSpeedLeftF1_Frame + BetaKR_FF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneR_Frame + BetaKL_FF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneL_Frame + BetaK_FF_LC * Vehicles[n].VehFrames[f].AverageSurroundingDensity_Frame + BetaRightLane_FF_LC * Vehicles[n].VehFrames[f].RightLaneDummy_Frame + BetaLeftLane_FF_LC * Vehicles[n].VehFrames[f].LeftLaneDummy_Frame + BetaRamp_FF_LC * Vehicles[n].VehFrames[f].RampDummy_Frame + BetaDistanceToRamp_FF_LC * Vehicles[n].VehFrames[f].DistanceToRamp_Frame)+ W_tmp );
                                //Hazard2_tmp = Hazard_tmp * Lambda_tmp;
                                
                                Vehicles[n].VehFrames[f].LaneChange_Hazard_FF = Hazard2_tmp;
                                
                                //Hazard: Free-Flow with Mode Exit
                                //__Assuming Exponential Distribution
                                
                                //____Number One: Limdep Technique
                                Lambda_tmp = exp ( -1.0 * ( BetaOne_FF_CM + BetaLCL_FF_CM * Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame + BetaV_FF_CM * Vehicles[n].VehFrames[f].DriverSpeed_Frame + BetaDXL1_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeader1_Frame + BetaDVL1_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame + BetaDXL2_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeader2_Frame + BetaDVL2_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeader2_Frame + BetaDXF1_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayFollower1_Frame + BetaDVF1_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedFollower1_Frame + BetaDXL1R_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame + BetaDVL1R_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame + BetaDXF1R_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame + BetaDVF1R_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame + BetaDXL1L_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeftL1_Frame + BetaDVL1L_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeftL1_Frame + BetaDXF1L_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeftF1_Frame + BetaDVF1L_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeftF1_Frame + BetaKR_FF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneR_Frame + BetaKL_FF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneL_Frame + BetaK_FF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensity_Frame + BetaRightLane_FF_CM * Vehicles[n].VehFrames[f].RightLaneDummy_Frame + BetaLeftLane_FF_CM * Vehicles[n].VehFrames[f].LeftLaneDummy_Frame + BetaRamp_FF_CM * Vehicles[n].VehFrames[f].RampDummy_Frame + BetaDistanceToRamp_FF_CM * Vehicles[n].VehFrames[f].DistanceToRamp_Frame) );
                                
                                Hazard_ParameterA_FF_CM = Lambda_tmp;
                                Hazard_ParameterB_FF_CM = 1.0;//Difference Between Exponential and Weibull
                                Hazard_tmp = Hazard_ParameterA_FF_CM * Hazard_ParameterB_FF_CM * pow ( ( Hazard_ParameterA_FF_CM * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ),( Hazard_ParameterB_FF_CM - 1 ) );
                                Survival_tmp = exp (  -1.0 * pow ( ( Hazard_ParameterA_FF_CM * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ) , Hazard_ParameterB_FF_CM )  );
                                
                                R_Heterogeneity_FF_CM = K_Heterogeneity_FF_CM;
                                
                                //Teta_tmp = ( 1.0 / K_Heterogeneity_FF_CM );
                                Teta_tmp = 0.55243252;
                                
                                Survival2_tmp = Survival_tmp * Teta_tmp;
                                
                                Hazard2_tmp = Survival2_tmp * Hazard_tmp;
                                
                                //____Number Two: Bhat Technique
                                //R_Heterogeneity_FF_CM = K_Heterogeneity_FF_CM;
                                //Hazard_ParameterB_FF_CM = 1.0;
                                //Hazard_tmp = Hazard_ParameterA_FF_CM * Hazard_ParameterB_FF_CM * pow ( ( Hazard_ParameterA_FF_CM * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ),( Hazard_ParameterB_FF_CM - 1 ) );
                                //Survival_tmp = exp (  -1.0 * pow ( ( Hazard_ParameterA_FF_CM * ( Vehicles[n].VehFrames[f].DurationTime_Frame * 0.1 ) ) , Hazard_ParameterB_FF_CM )  );
                                //
                                ////Wrong:Gamma_tmp = ( pow ( K_Heterogeneity_FF_CM, R_Heterogeneity_FF_CM ) / double ( factorial ( int ( R_Heterogeneity_FF_CM - 1 ) ) ) ) * exp ( - 1.0 * K_Heterogeneity_FF_CM * V_tmp) * pow ( V_tmp , ( R_Heterogeneity_FF_CM - 1)  );
                                //
                                ////Gamma_tmp = Vehicles[n].VehFrames[f].GammaHeterogeneity_FF_CM;
                                //W_tmp = log (Gamma_tmp);//(?): What is v
                                //Lambda_tmp = exp ( -1.0 * ( BetaOne_FF_CM + BetaLCL_FF_CM * Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame + BetaV_FF_CM * Vehicles[n].VehFrames[f].DriverSpeed_Frame + BetaDXL1_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeader1_Frame + BetaDVL1_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame + BetaDXL2_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeader2_Frame + BetaDVL2_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeader2_Frame + BetaDXF1_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayFollower1_Frame + BetaDVF1_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedFollower1_Frame + BetaDXL1R_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame + BetaDVL1R_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame + BetaDXF1R_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame + BetaDVF1R_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame + BetaDXL1L_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeftL1_Frame + BetaDVL1L_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeftL1_Frame + BetaDXF1L_FF_CM * Vehicles[n].VehFrames[f].DistanceHeadwayLeftF1_Frame + BetaDVF1L_FF_CM * Vehicles[n].VehFrames[f].RelativeSpeedLeftF1_Frame + BetaKR_FF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneR_Frame + BetaKL_FF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneL_Frame + BetaK_FF_CM * Vehicles[n].VehFrames[f].AverageSurroundingDensity_Frame + BetaRightLane_FF_CM * Vehicles[n].VehFrames[f].RightLaneDummy_Frame + BetaLeftLane_FF_CM * Vehicles[n].VehFrames[f].LeftLaneDummy_Frame + BetaRamp_FF_CM * Vehicles[n].VehFrames[f].RampDummy_Frame + BetaDistanceToRamp_FF_CM * Vehicles[n].VehFrames[f].DistanceToRamp_Frame)+ W_tmp );
                                //Hazard2_tmp = Hazard_tmp * Lambda_tmp;
                                
                                Vehicles[n].VehFrames[f].Mode_Hazard_FF = Hazard2_tmp;
                                
                                //Mode Choice: Duration Delimitation
                                Mode_Percentage = ( (rand ()%1000) / 1000.0 );
                                
                                //Test: No Effect if Missing Data
                                if (Vehicles[n].VehFrames[f].Flag1 == 1 || Vehicles[n].VehFrames[f].Flag2 == 1)
                                {
                                    Vehicles[n].VehFrames[f].LaneChange_Hazard_CF = 0.0;
                                    Vehicles[n].VehFrames[f].LaneChange_Hazard_FF = 0.0;
                                    Vehicles[n].VehFrames[f].Mode_Hazard_CF = 0.0;
                                    Vehicles[n].VehFrames[f].Mode_Hazard_FF = 0.0;
                                }
                                
                                {
                                    if (f == 0)
                                    {
                                        //Considered as CF Episode: 1st Episode
                                        if (Mode_Percentage < Vehicles[n].VehFrames[f].Mode_Hazard_CF)//New Episode
                                        {
                                            RT_Counter = 0;
                                            
                                            Vehicles[n].VehFrames[f].Mode = 0;
                                            Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame = 0;
                                        }
                                        else
                                        {
                                            Vehicles[n].VehFrames[f].Mode = 1;
                                        }
                                        
                                    }
                                    else if (Vehicles[n].VehFrames[f].Mode_Previous == 1)
                                    {
                                        //CF Episode
                                        if (Mode_Percentage < Vehicles[n].VehFrames[f].Mode_Hazard_CF)//New Episode
                                        {
                                            RT_Counter = 0;
                                            
                                            Vehicles[n].VehFrames[f].Mode = 0;
                                            Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame = 0;
                                        }
                                        else
                                        {
                                            Vehicles[n].VehFrames[f].Mode = 1;
                                        }
                                    }
                                    else if (Vehicles[n].VehFrames[f].Mode_Previous == 0)
                                    {
                                        //FF Episode
                                        if (Mode_Percentage < Vehicles[n].VehFrames[f].Mode_Hazard_FF)//New Episode
                                        {
                                            RT_Counter = 0;
                                            
                                            Vehicles[n].VehFrames[f].Mode = 1;
                                            Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame = 0;
                                        }
                                        else
                                        {
                                            Vehicles[n].VehFrames[f].Mode = 0;
                                        }
                                    }
                                }
                                
                                //____________________________End of Added by Samer Hamdar: 8 October 2008
                                
                                Vehicles[n].VehFrames[f].WallstenAcceleration = At_Update;
                                Vehicles[n].VehFrames[f].WallstenVelocity = Vt_Update;
                                Vehicles[n].VehFrames[f].WallstenX = Xt_Update;
                                Vehicles[n].VehFrames[f].WallstenRelativeSpeed = DeltaV_Update;
                                Vehicles[n].VehFrames[f].WallstenSpaceHeadway = DeltaX_Update;
                                Vehicles[n].VehFrames[f].WallstenGap = S_Update;//Possible Error: Problem Since Space Gaps are Sometimes Negative: Use Space Headway Instead
                                
                                FrameTime[x_c].FrameLocation[y_c].WallstenVelocity = Vehicles[n].VehFrames[f].WallstenVelocity;
                                FrameTime[x_c].FrameLocation[y_c].WallstenX = Vehicles[n].VehFrames[f].WallstenX;
                                FrameTime[x_c].FrameLocation[y_c].LeadersChangingLanes_Frame = Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame;
                                
                                //End of Modified by Samer Hamdar: 8 September 2008
                            }
                            
                            Vehicles[n].VehFrames[f].DummyNoLeader = 0;
                            Vehicles[n].VehFrames[f].DummyWallstenMeasure = 999.0;
                            
                            //Error Computation: for the time being: it is for velocity
                            //ErrorDummy1_V = ErrorDummy1_V + pow ( ( ( Vehicles[n].VehFrames[f].WallstenVelocity - Vehicles[n].VehFrames[f+1].VehicleVelocity ) / Vehicles[n].VehFrames[f+1].VehicleVelocity) , 2.0 ) ;
                            //ErrorDummy2_V = ErrorDummy2_V + pow (  ( Vehicles[n].VehFrames[f].WallstenVelocity - Vehicles[n].VehFrames[f+1].VehicleVelocity ) , 2.0 ) ;
                            //ErrorDummy3_V = ErrorDummy3_V + fabs ( Vehicles[n].VehFrames[f+1].VehicleVelocity ) ;
                            //ErrorDummy4_V = ErrorDummy4_V + ( pow (  ( Vehicles[n].VehFrames[f].WallstenVelocity - Vehicles[n].VehFrames[f+1].VehicleVelocity ) , 2.0 ) / fabs ( Vehicles[n].VehFrames[f+1].VehicleVelocity ) ) ;
                            //ErrorDummy5_V = ErrorDummy5_V + ( Vehicles[n].VehFrames[f+1].VehicleVelocity );
                            
                            if (f < 0.8 * (Vehicles[n].NumFrames - 1))
                            {
                                //Error Terms Update: Velocity
                                if (Vehicles[n].VehFrames[f+1].VehicleVelocity == 0)
                                    ErrorDummy1_V = ErrorDummy1_V + pow ( ( ( Vehicles[n].VehFrames[f].WallstenVelocity - Vehicles[n].VehFrames[f+1].VehicleVelocity ) / 0.1) , 2.0 ) ;
                                else
                                    ErrorDummy1_V = ErrorDummy1_V + pow ( ( ( Vehicles[n].VehFrames[f].WallstenVelocity - Vehicles[n].VehFrames[f+1].VehicleVelocity ) / Vehicles[n].VehFrames[f+1].VehicleVelocity) , 2.0 ) ;
                                
                                ErrorDummy2_V = ErrorDummy2_V + pow (  ( Vehicles[n].VehFrames[f].WallstenVelocity - Vehicles[n].VehFrames[f+1].VehicleVelocity ) , 2.0 ) ;
                                
                                ErrorDummy3_V = ErrorDummy3_V + fabs ( Vehicles[n].VehFrames[f+1].VehicleVelocity ) ;
                                
                                if (Vehicles[n].VehFrames[f+1].VehicleVelocity == 0)
                                    ErrorDummy4_V = ErrorDummy4_V + ( pow (  ( Vehicles[n].VehFrames[f].WallstenVelocity - Vehicles[n].VehFrames[f+1].VehicleVelocity ) , 2.0 ) / fabs ( 0.1 ) ) ;
                                else
                                    ErrorDummy4_V = ErrorDummy4_V + ( pow (  ( Vehicles[n].VehFrames[f].WallstenVelocity - Vehicles[n].VehFrames[f+1].VehicleVelocity ) , 2.0 ) / fabs ( Vehicles[n].VehFrames[f+1].VehicleVelocity ) ) ;
                                
                                ErrorDummy5_V = ErrorDummy5_V + ( Vehicles[n].VehFrames[f+1].VehicleVelocity );
                                
                                
                                //Error Terms Update: Space Headway
                                if (Vehicles[n].VehFrames[f+1].SpaceGap == 0)
                                    ErrorDummy1_S = ErrorDummy1_S + pow ( ( ( Vehicles[n].VehFrames[f].WallstenGap - Vehicles[n].VehFrames[f+1].SpaceGap ) / 0.1) , 2.0 ) ;
                                else
                                    ErrorDummy1_S = ErrorDummy1_S + pow ( ( ( Vehicles[n].VehFrames[f].WallstenGap - Vehicles[n].VehFrames[f+1].SpaceGap ) / Vehicles[n].VehFrames[f+1].SpaceGap) , 2.0 ) ;
                                
                                ErrorDummy2_S = ErrorDummy2_S + pow (  ( Vehicles[n].VehFrames[f].WallstenGap - Vehicles[n].VehFrames[f+1].SpaceGap ) , 2.0 ) ;
                                
                                ErrorDummy3_S = ErrorDummy3_S + fabs ( Vehicles[n].VehFrames[f+1].SpaceGap ) ;
                                
                                if (Vehicles[n].VehFrames[f+1].SpaceGap == 0)
                                    ErrorDummy4_S = ErrorDummy4_S + ( pow (  ( Vehicles[n].VehFrames[f].WallstenGap - Vehicles[n].VehFrames[f+1].SpaceGap ) , 2.0 ) / fabs ( 0.1 ) ) ;
                                else
                                    ErrorDummy4_S = ErrorDummy4_S + ( pow (  ( Vehicles[n].VehFrames[f].WallstenGap - Vehicles[n].VehFrames[f+1].SpaceGap ) , 2.0 ) / fabs ( Vehicles[n].VehFrames[f+1].SpaceGap ) ) ;
                                
                                ErrorDummy5_S = ErrorDummy5_S + ( Vehicles[n].VehFrames[f+1].SpaceGap );
                                
                                
                                //Error Terms Update: Space Headway
                                if (Vehicles[n].VehFrames[f+1].RelativeVelocity == 0)
                                    ErrorDummy1_DV = ErrorDummy1_DV + pow ( ( ( Vehicles[n].VehFrames[f].WallstenRelativeSpeed - Vehicles[n].VehFrames[f+1].RelativeVelocity ) / 0.1) , 2.0 ) ;
                                else
                                    ErrorDummy1_DV = ErrorDummy1_DV + pow ( ( ( Vehicles[n].VehFrames[f].WallstenRelativeSpeed - Vehicles[n].VehFrames[f+1].RelativeVelocity ) / Vehicles[n].VehFrames[f+1].RelativeVelocity) , 2.0 ) ;
                                
                                ErrorDummy2_DV = ErrorDummy2_DV + pow (  ( Vehicles[n].VehFrames[f].WallstenRelativeSpeed - Vehicles[n].VehFrames[f+1].RelativeVelocity ) , 2.0 ) ;
                                
                                ErrorDummy3_DV = ErrorDummy3_DV + fabs ( Vehicles[n].VehFrames[f+1].RelativeVelocity ) ;
                                
                                if (Vehicles[n].VehFrames[f+1].RelativeVelocity == 0)
                                    ErrorDummy4_DV = ErrorDummy4_DV + ( pow (  ( Vehicles[n].VehFrames[f].WallstenRelativeSpeed - Vehicles[n].VehFrames[f+1].RelativeVelocity ) , 2.0 ) / fabs ( 0.1 ) ) ;
                                else
                                    ErrorDummy4_DV = ErrorDummy4_DV + ( pow (  ( Vehicles[n].VehFrames[f].WallstenRelativeSpeed - Vehicles[n].VehFrames[f+1].RelativeVelocity ) , 2.0 ) / fabs ( Vehicles[n].VehFrames[f+1].RelativeVelocity ) ) ;
                                
                                ErrorDummy5_DV = ErrorDummy5_DV + ( Vehicles[n].VehFrames[f+1].RelativeVelocity );
                                
                                
                                RT_Counter = RT_Counter + 1;
                            }
                            else
                            {
                                //Error Terms Update: Velocity
                                if (Vehicles[n].VehFrames[f+1].VehicleVelocity == 0)
                                    ErrorDummy1_V_Validation = ErrorDummy1_V_Validation + pow ( ( ( Vehicles[n].VehFrames[f].WallstenVelocity - Vehicles[n].VehFrames[f+1].VehicleVelocity ) / 0.1) , 2.0 ) ;
                                else
                                    ErrorDummy1_V_Validation = ErrorDummy1_V_Validation + pow ( ( ( Vehicles[n].VehFrames[f].WallstenVelocity - Vehicles[n].VehFrames[f+1].VehicleVelocity ) / Vehicles[n].VehFrames[f+1].VehicleVelocity) , 2.0 ) ;
                                
                                ErrorDummy2_V_Validation = ErrorDummy2_V_Validation + pow (  ( Vehicles[n].VehFrames[f].WallstenVelocity - Vehicles[n].VehFrames[f+1].VehicleVelocity ) , 2.0 ) ;
                                
                                ErrorDummy3_V_Validation = ErrorDummy3_V_Validation + fabs ( Vehicles[n].VehFrames[f+1].VehicleVelocity ) ;
                                
                                if (Vehicles[n].VehFrames[f+1].VehicleVelocity == 0)
                                    ErrorDummy4_V_Validation = ErrorDummy4_V_Validation + ( pow (  ( Vehicles[n].VehFrames[f].WallstenVelocity - Vehicles[n].VehFrames[f+1].VehicleVelocity ) , 2.0 ) / fabs ( 0.1 ) ) ;
                                else
                                    ErrorDummy4_V_Validation = ErrorDummy4_V_Validation + ( pow (  ( Vehicles[n].VehFrames[f].WallstenVelocity - Vehicles[n].VehFrames[f+1].VehicleVelocity ) , 2.0 ) / fabs ( Vehicles[n].VehFrames[f+1].VehicleVelocity ) ) ;
                                
                                ErrorDummy5_V_Validation = ErrorDummy5_V_Validation + ( Vehicles[n].VehFrames[f+1].VehicleVelocity );
                                
                                
                                //Error Terms Update: Space Headway
                                if (Vehicles[n].VehFrames[f+1].SpaceGap == 0)
                                    ErrorDummy1_S_Validation = ErrorDummy1_S_Validation + pow ( ( ( Vehicles[n].VehFrames[f].WallstenGap - Vehicles[n].VehFrames[f+1].SpaceGap ) / 0.1) , 2.0 ) ;
                                else
                                    ErrorDummy1_S_Validation = ErrorDummy1_S_Validation + pow ( ( ( Vehicles[n].VehFrames[f].WallstenGap - Vehicles[n].VehFrames[f+1].SpaceGap ) / Vehicles[n].VehFrames[f+1].SpaceGap) , 2.0 ) ;
                                
                                ErrorDummy2_S_Validation = ErrorDummy2_S_Validation + pow (  ( Vehicles[n].VehFrames[f].WallstenGap - Vehicles[n].VehFrames[f+1].SpaceGap ) , 2.0 ) ;
                                
                                ErrorDummy3_S_Validation = ErrorDummy3_S_Validation + fabs ( Vehicles[n].VehFrames[f+1].SpaceGap ) ;
                                
                                if (Vehicles[n].VehFrames[f+1].SpaceGap == 0)
                                    ErrorDummy4_S_Validation = ErrorDummy4_S_Validation + ( pow (  ( Vehicles[n].VehFrames[f].WallstenGap - Vehicles[n].VehFrames[f+1].SpaceGap ) , 2.0 ) / fabs ( 0.1 ) ) ;
                                else
                                    ErrorDummy4_S_Validation = ErrorDummy4_S_Validation + ( pow (  ( Vehicles[n].VehFrames[f].WallstenGap - Vehicles[n].VehFrames[f+1].SpaceGap ) , 2.0 ) / fabs ( Vehicles[n].VehFrames[f+1].SpaceGap ) ) ;
                                
                                ErrorDummy5_S_Validation = ErrorDummy5_S_Validation + ( Vehicles[n].VehFrames[f+1].SpaceGap );
                                
                                
                                //Error Terms Update: Space Headway
                                if (Vehicles[n].VehFrames[f+1].RelativeVelocity == 0)
                                    ErrorDummy1_DV_Validation = ErrorDummy1_DV_Validation + pow ( ( ( Vehicles[n].VehFrames[f].WallstenRelativeSpeed - Vehicles[n].VehFrames[f+1].RelativeVelocity ) / 0.1) , 2.0 ) ;
                                else
                                    ErrorDummy1_DV_Validation = ErrorDummy1_DV_Validation + pow ( ( ( Vehicles[n].VehFrames[f].WallstenRelativeSpeed - Vehicles[n].VehFrames[f+1].RelativeVelocity ) / Vehicles[n].VehFrames[f+1].RelativeVelocity) , 2.0 ) ;
                                
                                ErrorDummy2_DV_Validation = ErrorDummy2_DV_Validation + pow (  ( Vehicles[n].VehFrames[f].WallstenRelativeSpeed - Vehicles[n].VehFrames[f+1].RelativeVelocity ) , 2.0 ) ;
                                
                                ErrorDummy3_DV_Validation = ErrorDummy3_DV_Validation + fabs ( Vehicles[n].VehFrames[f+1].RelativeVelocity ) ;
                                
                                if (Vehicles[n].VehFrames[f+1].RelativeVelocity == 0)
                                    ErrorDummy4_DV_Validation = ErrorDummy4_DV_Validation + ( pow (  ( Vehicles[n].VehFrames[f].WallstenRelativeSpeed - Vehicles[n].VehFrames[f+1].RelativeVelocity ) , 2.0 ) / fabs ( 0.1 ) ) ;
                                else
                                    ErrorDummy4_DV_Validation = ErrorDummy4_DV_Validation + ( pow (  ( Vehicles[n].VehFrames[f].WallstenRelativeSpeed - Vehicles[n].VehFrames[f+1].RelativeVelocity ) , 2.0 ) / fabs ( Vehicles[n].VehFrames[f+1].RelativeVelocity ) ) ;
                                
                                ErrorDummy5_DV_Validation = ErrorDummy5_DV_Validation + ( Vehicles[n].VehFrames[f+1].RelativeVelocity );
                                
                                
                                RT_Counter = RT_Counter + 1;
                            }
                            
                        }
                        else//if there is no leader: Put some value into the Wallsten Measures and Re-Initialize Yt = 0
                        {
                            Yt = 0.0;
                            
                            Vehicles[n].VehFrames[f].WallstenAcceleration = Vehicles[n].VehFrames[f].VehicleAcceleration;
                            Vehicles[n].VehFrames[f].WallstenVelocity = Vehicles[n].VehFrames[f].VehicleVelocity;
                            Vehicles[n].VehFrames[f].WallstenX = Vehicles[n].VehFrames[f].LocalY;
                            Vehicles[n].VehFrames[f].WallstenSpaceHeadway = Vehicles[n].VehFrames[f].SpaceHeadway;
                            Vehicles[n].VehFrames[f].WallstenRelativeSpeed = Vehicles[n].VehFrames[f].RelativeVelocity;
                            Vehicles[n].VehFrames[f].WallstenGap = Vehicles[n].VehFrames[f].SpaceGap;
                            
                            Vehicles[n].VehFrames[f].DummyNoLeader = 1;
                            Vehicles[n].VehFrames[f].DummyWallstenMeasure = - 0.1;
                            
                            RT_Counter = 0;
                        }
                        //__________________________________________________________________________________________
                    }
                    else// Accident Detected
                    {
                        Vehicles[n].VehFrames[f].WallstenAcceleration = 0.0;
                        Vehicles[n].VehFrames[f].WallstenVelocity = 0.1;
                        Vehicles[n].VehFrames[f].WallstenX = Vehicles[n].VehFrames[f-1].WallstenX;
                        Vehicles[n].VehFrames[f].WallstenSpaceHeadway = 0.1;
                        Vehicles[n].VehFrames[f].WallstenRelativeSpeed = 0.1;
                        Vehicles[n].VehFrames[f].WallstenGap = 0.1;
                        
                        //Added by Samer Hamdar: 14 October 2008
                        Vehicles[n].VehFrames[f].Mode = -2;
                        Vehicles[n].VehFrames[f].LaneChange_Hazard_CF = -0.1;
                        Vehicles[n].VehFrames[f].LaneChange_Hazard_FF = -0.1;
                        Vehicles[n].VehFrames[f].Mode_Hazard_CF = -0.1;
                        Vehicles[n].VehFrames[f].Mode_Hazard_FF = -0.1;
                        
                        Vehicles[n].VehFrames[f].AverageSurroundingDensity_Frame = 0.2;
                        Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneL_Frame = 0.2;
                        Vehicles[n].VehFrames[f].AverageSurroundingDensityLaneR_Frame = 0.2;
                        Vehicles[n].VehFrames[f].DistanceHeadwayFollower1_Frame = 0.2;
                        Vehicles[n].VehFrames[f].DistanceHeadwayLeader1_Frame = 0.2;
                        Vehicles[n].VehFrames[f].DistanceHeadwayLeader2_Frame = 0.2;
                        Vehicles[n].VehFrames[f].DistanceHeadwayLeftF1_Frame = 0.2;
                        Vehicles[n].VehFrames[f].DistanceHeadwayLeftL1_Frame = 0.2;
                        Vehicles[n].VehFrames[f].DistanceHeadwayRightF1_Frame = 0.2;
                        Vehicles[n].VehFrames[f].DistanceHeadwayRightL1_Frame = 0.2;
                        Vehicles[n].VehFrames[f].DistancetoEntrance_Frame = 0.2;
                        Vehicles[n].VehFrames[f].DistanceToRamp_Frame = 0.2;
                        Vehicles[n].VehFrames[f].DriverSpeed_Frame = 0.2;
                        Vehicles[n].VehFrames[f].LeadersChangingLanes_Frame = 0.2;
                        Vehicles[n].VehFrames[f].LeftLaneDummy_Frame = 0.2;
                        Vehicles[n].VehFrames[f].RampDummy_Frame = 0.2;
                        Vehicles[n].VehFrames[f].RelativeSpeedFollower1_Frame = 0.2;
                        Vehicles[n].VehFrames[f].RelativeSpeedLeader1_Frame = 0.2;
                        Vehicles[n].VehFrames[f].RelativeSpeedLeader2_Frame = 0.2;
                        Vehicles[n].VehFrames[f].RelativeSpeedLeftF1_Frame = 0.2;
                        Vehicles[n].VehFrames[f].RelativeSpeedLeftL1_Frame = 0.2;
                        Vehicles[n].VehFrames[f].RelativeSpeedRightF1_Frame = 0.2;
                        Vehicles[n].VehFrames[f].RelativeSpeedRightL1_Frame = 0.2;
                        Vehicles[n].VehFrames[f].RightLaneDummy_Frame =0.2;
                        Vehicles[n].VehFrames[f].VehicleType_Frame = 0.2;
                        //End of Added by Samer Hamdar: 14 October 2008
                        
                        if (f < 0.8 * (Vehicles[n].NumFrames - 1))
                        {
                            //Error Terms Update: Velocity
                            if (Vehicles[n].VehFrames[f+1].VehicleVelocity == 0)
                                ErrorDummy1_V = ErrorDummy1_V + pow ( ( ( Vehicles[n].VehFrames[f].WallstenVelocity - Vehicles[n].VehFrames[f+1].VehicleVelocity ) / 0.1) , 2.0 ) ;
                            else
                                ErrorDummy1_V = ErrorDummy1_V + pow ( ( ( Vehicles[n].VehFrames[f].WallstenVelocity - Vehicles[n].VehFrames[f+1].VehicleVelocity ) / Vehicles[n].VehFrames[f+1].VehicleVelocity) , 2.0 ) ;
                            
                            ErrorDummy2_V = ErrorDummy2_V + pow (  ( Vehicles[n].VehFrames[f].WallstenVelocity - Vehicles[n].VehFrames[f+1].VehicleVelocity ) , 2.0 ) ;
                            
                            ErrorDummy3_V = ErrorDummy3_V + fabs ( Vehicles[n].VehFrames[f+1].VehicleVelocity ) ;
                            
                            if (Vehicles[n].VehFrames[f+1].VehicleVelocity == 0)
                                ErrorDummy4_V = ErrorDummy4_V + ( pow (  ( Vehicles[n].VehFrames[f].WallstenVelocity - Vehicles[n].VehFrames[f+1].VehicleVelocity ) , 2.0 ) / fabs ( 0.1 ) ) ;
                            else
                                ErrorDummy4_V = ErrorDummy4_V + ( pow (  ( Vehicles[n].VehFrames[f].WallstenVelocity - Vehicles[n].VehFrames[f+1].VehicleVelocity ) , 2.0 ) / fabs ( Vehicles[n].VehFrames[f+1].VehicleVelocity ) ) ;
                            
                            ErrorDummy5_V = ErrorDummy5_V + ( Vehicles[n].VehFrames[f+1].VehicleVelocity );
                            
                            
                            //Error Terms Update: Space Headway
                            if (Vehicles[n].VehFrames[f+1].SpaceGap == 0)
                                ErrorDummy1_S = ErrorDummy1_S + pow ( ( ( Vehicles[n].VehFrames[f].WallstenGap - Vehicles[n].VehFrames[f+1].SpaceGap ) / 0.1) , 2.0 ) ;
                            else
                                ErrorDummy1_S = ErrorDummy1_S + pow ( ( ( Vehicles[n].VehFrames[f].WallstenGap - Vehicles[n].VehFrames[f+1].SpaceGap ) / Vehicles[n].VehFrames[f+1].SpaceGap) , 2.0 ) ;
                            
                            ErrorDummy2_S = ErrorDummy2_S + pow (  ( Vehicles[n].VehFrames[f].WallstenGap - Vehicles[n].VehFrames[f+1].SpaceGap ) , 2.0 ) ;
                            
                            ErrorDummy3_S = ErrorDummy3_S + fabs ( Vehicles[n].VehFrames[f+1].SpaceGap ) ;
                            
                            if (Vehicles[n].VehFrames[f+1].SpaceGap == 0)
                                ErrorDummy4_S = ErrorDummy4_S + ( pow (  ( Vehicles[n].VehFrames[f].WallstenGap - Vehicles[n].VehFrames[f+1].SpaceGap ) , 2.0 ) / fabs ( 0.1 ) ) ;
                            else
                                ErrorDummy4_S = ErrorDummy4_S + ( pow (  ( Vehicles[n].VehFrames[f].WallstenGap - Vehicles[n].VehFrames[f+1].SpaceGap ) , 2.0 ) / fabs ( Vehicles[n].VehFrames[f+1].SpaceGap ) ) ;
                            
                            ErrorDummy5_S = ErrorDummy5_S + ( Vehicles[n].VehFrames[f+1].SpaceGap );
                            
                            
                            //Error Terms Update: Space Headway
                            if (Vehicles[n].VehFrames[f+1].RelativeVelocity == 0)
                                ErrorDummy1_DV = ErrorDummy1_DV + pow ( ( ( Vehicles[n].VehFrames[f].WallstenRelativeSpeed - Vehicles[n].VehFrames[f+1].RelativeVelocity ) / 0.1) , 2.0 ) ;
                            else
                                ErrorDummy1_DV = ErrorDummy1_DV + pow ( ( ( Vehicles[n].VehFrames[f].WallstenRelativeSpeed - Vehicles[n].VehFrames[f+1].RelativeVelocity ) / Vehicles[n].VehFrames[f+1].RelativeVelocity) , 2.0 ) ;
                            
                            ErrorDummy2_DV = ErrorDummy2_DV + pow (  ( Vehicles[n].VehFrames[f].WallstenRelativeSpeed - Vehicles[n].VehFrames[f+1].RelativeVelocity ) , 2.0 ) ;
                            
                            ErrorDummy3_DV = ErrorDummy3_DV + fabs ( Vehicles[n].VehFrames[f+1].RelativeVelocity ) ;
                            
                            if (Vehicles[n].VehFrames[f+1].RelativeVelocity == 0)
                                ErrorDummy4_DV = ErrorDummy4_DV + ( pow (  ( Vehicles[n].VehFrames[f].WallstenRelativeSpeed - Vehicles[n].VehFrames[f+1].RelativeVelocity ) , 2.0 ) / fabs ( 0.1 ) ) ;
                            else
                                ErrorDummy4_DV = ErrorDummy4_DV + ( pow (  ( Vehicles[n].VehFrames[f].WallstenRelativeSpeed - Vehicles[n].VehFrames[f+1].RelativeVelocity ) , 2.0 ) / fabs ( Vehicles[n].VehFrames[f+1].RelativeVelocity ) ) ;
                            
                            ErrorDummy5_DV = ErrorDummy5_DV + ( Vehicles[n].VehFrames[f+1].RelativeVelocity );
                        }
                        else
                        {
                            //Error Terms Update: Velocity
                            if (Vehicles[n].VehFrames[f+1].VehicleVelocity == 0)
                                ErrorDummy1_V_Validation = ErrorDummy1_V_Validation + pow ( ( ( Vehicles[n].VehFrames[f].WallstenVelocity - Vehicles[n].VehFrames[f+1].VehicleVelocity ) / 0.1) , 2.0 ) ;
                            else
                                ErrorDummy1_V_Validation = ErrorDummy1_V_Validation + pow ( ( ( Vehicles[n].VehFrames[f].WallstenVelocity - Vehicles[n].VehFrames[f+1].VehicleVelocity ) / Vehicles[n].VehFrames[f+1].VehicleVelocity) , 2.0 ) ;
                            
                            ErrorDummy2_V_Validation = ErrorDummy2_V_Validation + pow (  ( Vehicles[n].VehFrames[f].WallstenVelocity - Vehicles[n].VehFrames[f+1].VehicleVelocity ) , 2.0 ) ;
                            
                            ErrorDummy3_V_Validation = ErrorDummy3_V_Validation + fabs ( Vehicles[n].VehFrames[f+1].VehicleVelocity ) ;
                            
                            if (Vehicles[n].VehFrames[f+1].VehicleVelocity == 0)
                                ErrorDummy4_V_Validation = ErrorDummy4_V_Validation + ( pow (  ( Vehicles[n].VehFrames[f].WallstenVelocity - Vehicles[n].VehFrames[f+1].VehicleVelocity ) , 2.0 ) / fabs ( 0.1 ) ) ;
                            else
                                ErrorDummy4_V_Validation = ErrorDummy4_V_Validation + ( pow (  ( Vehicles[n].VehFrames[f].WallstenVelocity - Vehicles[n].VehFrames[f+1].VehicleVelocity ) , 2.0 ) / fabs ( Vehicles[n].VehFrames[f+1].VehicleVelocity ) ) ;
                            
                            ErrorDummy5_V_Validation = ErrorDummy5_V_Validation + ( Vehicles[n].VehFrames[f+1].VehicleVelocity );
                            
                            
                            //Error Terms Update: Space Headway
                            if (Vehicles[n].VehFrames[f+1].SpaceGap == 0)
                                ErrorDummy1_S_Validation = ErrorDummy1_S_Validation + pow ( ( ( Vehicles[n].VehFrames[f].WallstenGap - Vehicles[n].VehFrames[f+1].SpaceGap ) / 0.1) , 2.0 ) ;
                            else
                                ErrorDummy1_S_Validation = ErrorDummy1_S_Validation + pow ( ( ( Vehicles[n].VehFrames[f].WallstenGap - Vehicles[n].VehFrames[f+1].SpaceGap ) / Vehicles[n].VehFrames[f+1].SpaceGap) , 2.0 ) ;
                            
                            ErrorDummy2_S_Validation = ErrorDummy2_S_Validation + pow (  ( Vehicles[n].VehFrames[f].WallstenGap - Vehicles[n].VehFrames[f+1].SpaceGap ) , 2.0 ) ;
                            
                            ErrorDummy3_S_Validation = ErrorDummy3_S_Validation + fabs ( Vehicles[n].VehFrames[f+1].SpaceGap ) ;
                            
                            if (Vehicles[n].VehFrames[f+1].SpaceGap == 0)
                                ErrorDummy4_S_Validation = ErrorDummy4_S_Validation + ( pow (  ( Vehicles[n].VehFrames[f].WallstenGap - Vehicles[n].VehFrames[f+1].SpaceGap ) , 2.0 ) / fabs ( 0.1 ) ) ;
                            else
                                ErrorDummy4_S_Validation = ErrorDummy4_S_Validation + ( pow (  ( Vehicles[n].VehFrames[f].WallstenGap - Vehicles[n].VehFrames[f+1].SpaceGap ) , 2.0 ) / fabs ( Vehicles[n].VehFrames[f+1].SpaceGap ) ) ;
                            
                            ErrorDummy5_S_Validation = ErrorDummy5_S_Validation + ( Vehicles[n].VehFrames[f+1].SpaceGap );
                            
                            
                            //Error Terms Update: Space Headway
                            if (Vehicles[n].VehFrames[f+1].RelativeVelocity == 0)
                                ErrorDummy1_DV_Validation = ErrorDummy1_DV_Validation + pow ( ( ( Vehicles[n].VehFrames[f].WallstenRelativeSpeed - Vehicles[n].VehFrames[f+1].RelativeVelocity ) / 0.1) , 2.0 ) ;
                            else
                                ErrorDummy1_DV_Validation = ErrorDummy1_DV_Validation + pow ( ( ( Vehicles[n].VehFrames[f].WallstenRelativeSpeed - Vehicles[n].VehFrames[f+1].RelativeVelocity ) / Vehicles[n].VehFrames[f+1].RelativeVelocity) , 2.0 ) ;
                            
                            ErrorDummy2_DV_Validation = ErrorDummy2_DV_Validation + pow (  ( Vehicles[n].VehFrames[f].WallstenRelativeSpeed - Vehicles[n].VehFrames[f+1].RelativeVelocity ) , 2.0 ) ;
                            
                            ErrorDummy3_DV_Validation = ErrorDummy3_DV_Validation + fabs ( Vehicles[n].VehFrames[f+1].RelativeVelocity ) ;
                            
                            if (Vehicles[n].VehFrames[f+1].RelativeVelocity == 0)
                                ErrorDummy4_DV_Validation = ErrorDummy4_DV_Validation + ( pow (  ( Vehicles[n].VehFrames[f].WallstenRelativeSpeed - Vehicles[n].VehFrames[f+1].RelativeVelocity ) , 2.0 ) / fabs ( 0.1 ) ) ;
                            else
                                ErrorDummy4_DV_Validation = ErrorDummy4_DV_Validation + ( pow (  ( Vehicles[n].VehFrames[f].WallstenRelativeSpeed - Vehicles[n].VehFrames[f+1].RelativeVelocity ) , 2.0 ) / fabs ( Vehicles[n].VehFrames[f+1].RelativeVelocity ) ) ;
                            
                            ErrorDummy5_DV_Validation = ErrorDummy5_DV_Validation + ( Vehicles[n].VehFrames[f+1].RelativeVelocity );
                        }
                    }
                    //________________________________________________________________________
                    
                    Leader_Previous = Vehicles[n].VehFrames[f].PreVehID;
                    Lane_Previous = Vehicles[n].VehFrames[f].Lane;
                }
                
                Vehicles[n].VehParameterChildren_Mixed_S[i].AbsoluteError_Velocity = sqrt ( ErrorDummy1_V );
                Vehicles[n].VehParameterChildren_Mixed_S[i].RelativeError_Velocity = sqrt ( ErrorDummy2_V / pow ( ErrorDummy5_V , 2.0 ) );
                Vehicles[n].VehParameterChildren_Mixed_S[i].MixedError_Velocity = sqrt ( ( 1.0 / ErrorDummy3_V ) * ErrorDummy4_V );
                
                Vehicles[n].VehParameterChildren_Mixed_S[i].AbsoluteError_Gap = sqrt ( ErrorDummy1_S );
                Vehicles[n].VehParameterChildren_Mixed_S[i].RelativeError_Gap = sqrt ( ErrorDummy2_S / pow ( ErrorDummy5_S , 2.0 ) );
                Vehicles[n].VehParameterChildren_Mixed_S[i].MixedError_Gap = sqrt ( ( 1.0 / ErrorDummy3_S ) * ErrorDummy4_S );
                
                Vehicles[n].VehParameterChildren_Mixed_S[i].AbsoluteError_RelativeVelocity = sqrt ( ErrorDummy1_DV );
                Vehicles[n].VehParameterChildren_Mixed_S[i].RelativeError_RelativeVelocity = sqrt ( ErrorDummy2_DV / pow ( ErrorDummy5_DV , 2.0 ) );
                Vehicles[n].VehParameterChildren_Mixed_S[i].MixedError_RelativeVelocity = sqrt ( ( 1.0 / ErrorDummy3_DV ) * ErrorDummy4_DV );
                
                
                Vehicles[n].VehParameterChildren_Mixed_S[i].AbsoluteError_Velocity_Validation = sqrt ( ErrorDummy1_V_Validation );
                Vehicles[n].VehParameterChildren_Mixed_S[i].RelativeError_Velocity_Validation = sqrt ( ErrorDummy2_V_Validation / pow ( ErrorDummy5_V_Validation , 2.0 ) );
                Vehicles[n].VehParameterChildren_Mixed_S[i].MixedError_Velocity_Validation = sqrt ( ( 1.0 / ErrorDummy3_V_Validation ) * ErrorDummy4_V_Validation );
                
                Vehicles[n].VehParameterChildren_Mixed_S[i].AbsoluteError_Gap_Validation = sqrt ( ErrorDummy1_S_Validation );
                Vehicles[n].VehParameterChildren_Mixed_S[i].RelativeError_Gap_Validation = sqrt ( ErrorDummy2_S_Validation / pow ( ErrorDummy5_S_Validation , 2.0 ) );
                Vehicles[n].VehParameterChildren_Mixed_S[i].MixedError_Gap_Validation = sqrt ( ( 1.0 / ErrorDummy3_S_Validation ) * ErrorDummy4_S_Validation );
                
                Vehicles[n].VehParameterChildren_Mixed_S[i].AbsoluteError_RelativeVelocity_Validation = sqrt ( ErrorDummy1_DV_Validation );
                Vehicles[n].VehParameterChildren_Mixed_S[i].RelativeError_RelativeVelocity_Validation = sqrt ( ErrorDummy2_DV_Validation / pow ( ErrorDummy5_DV_Validation , 2.0 ) );
                Vehicles[n].VehParameterChildren_Mixed_S[i].MixedError_RelativeVelocity_Validation = sqrt ( ( 1.0 / ErrorDummy3_DV_Validation ) * ErrorDummy4_DV_Validation );
            }
            
            //Rank with Respect to MixedError_Gap: Bubble Sort
            
            //Pseudo Code: procedure bubbleSort( A : list of sortable items ) defined as:
            //  for each i in 1 to length(A) do:
            //     for each j in length(A) downto i + 1 do:
            //       if A[ j -1  ] > A[ j ] then
            //         swap( A[ j - 1],  A[ j ] )
            //       end if
            //     end for
            //  end for
            //end procedure
            
            for (int x = 0; x < NumberOfChildren; x++)
            {
                for (int y = ( NumberOfChildren - 1 ); y > x; y--)
                {
                    if ( fabs ( Vehicles[n].VehParameterChildren_Mixed_S[y-1].MixedError_Velocity ) > fabs ( Vehicles[n].VehParameterChildren_Mixed_S[y].MixedError_Velocity)  )
                    {
                        veh_parameter_data_previous = Vehicles[n].VehParameterChildren_Mixed_S[y-1];
                        Vehicles[n].VehParameterChildren_Mixed_S[y-1] = Vehicles[n].VehParameterChildren_Mixed_S[y];
                        Vehicles[n].VehParameterChildren_Mixed_S[y] = veh_parameter_data_previous;
                        
                    }
                }
            }
            
            //Assign Top (Parent Number) to the Parent Vector Excluding Accident Related Parameters
            int s = 0;
            
            for (int t = 0; t < ( NumberOfChildren - 1 ); t++)
            {
                if (Vehicles[n].VehParameterChildren_Mixed_S[t].Accident == 0)
                {
                    if ( s < Parents)
                    {
                        Vehicles[n].VehParameter_Mixed_S[s] = Vehicles[n].VehParameterChildren_Mixed_S[t];
                        s = s + 1;
                    }
                }
            }
            
            
            //Carry the Best Chromosome to Next Step
            Vehicles[n].veh_parameter_data_best_mixed_S = Vehicles[n].VehParameter_Mixed_S[0];
            
            Error_Test_Previous = Error_Test;
            Error_Test = Vehicles[n].veh_parameter_data_best_mixed_S.MixedError_Velocity;
            
            //Check Condition
            if ( ( fabs (Error_Test_Previous - Error_Test ) ) <= 0.01)
            {
                Error_Counter = Error_Counter + 1;
            }
            else
            {
                Error_Counter = 0;
            }
        }
        
        // Here to End the While Loop for the Error - Mixed Error Gap
    }
    
    /*
     *****************************Re-retrieval of Duration Lengths/Types/Characteristics********************************
     //Added by Samer Hamdar: 15 October 2008
     NumberOfVehicles_Relative = 50;//NumberOfVehicles; 13-2=11
     NumberOfVehicles = 0;
     
     //________Retrieve Durations
     
     while( NumberOfVehicles < NumberOfVehicles_Relative - 2)
     {
     NumberOfFrames_Relative = 0;
     //End of Added by Samer Hamdar: 20 September 2008
     
     //duration data
     VehDurationData veh_duration_data;
     
     DurationTimetmp = 0;
     LeadersChangingLanestmp = 0; // Added by Samer Hamdar, March 23, 2007
     Vehicles[NumberOfVehicles].VehFrames[NumberOfFrames_Relative].NumberOfLaneChanges_D = LeadersChangingLanestmp;//Added by Samer Hamdar: 16 October 2008
     
     //veh
     Vehicles[NumberOfVehicles].NumDurations = 0;
     
     //Added by Samer Hamdar: 20 September 2008
     Vehicles[NumberOfVehicles].VehFrames[NumberOfFrames_Relative].VehIDFrame = Vehicles[NumberOfVehicles].VehID;
     
     Leader_Previous = 99999;
     RT_Counter = 0;
     
     FirstEpisodeDummytmp = 1; //Added by Samer Hamdar, March 23, 2007
     Vehicles[NumberOfVehicles].VehFrames[NumberOfFrames_Relative].FrameGlobalID = FrameGlobalIDtmp;//Added by Samer Hamdar, March 15, 2007
     FrameGlobalIDtmp = FrameGlobalIDtmp + 1;  //Added by Samer Hamdar, March 15, 2007
     
     NumberOfFrames = NumberOfFrames + 1; //Added by Samer Hamdar, March 19, 2007
     
     NumberOfFrames_Relative = NumberOfFrames_Relative + 1;//Added by Samer Hamdar: 20 September 2008
     
     // read other rows of a veh
     for (int i=1;i<Vehicles[NumberOfVehicles].NumFrames;i++)
     {
     Vehicles[NumberOfVehicles].VehFrames[NumberOfFrames_Relative].FrameGlobalID = FrameGlobalIDtmp;//Added by Samer Hamdar, March 15, 2007
     FrameGlobalIDtmp = FrameGlobalIDtmp + 1;  //Added by Samer Hamdar, March 15, 2007
     NumberOfFrames = NumberOfFrames + 1; //Added by Samer Hamdar, March 19, 2007
     
     DurationTimetmp = DurationTimetmp + (Vehicles[NumberOfVehicles].VehFrames[i].FrameID - Vehicles[NumberOfVehicles].VehFrames[i - 1].FrameID);
     
     //Re-Added by Samer Hamdar, March 23 (Added March 13), 2007: Save Last Record Instead of the First
     VehicleClasstmp = Vehicles[NumberOfVehicles].VehFrames[i - 1].VehicleClass;
     VehicleVelocitytmp = Vehicles[NumberOfVehicles].VehFrames[i - 1].VehicleVelocity;
     SpaceHeadwaytmp = Vehicles[NumberOfVehicles].VehFrames[i - 1].SpaceHeadway;
     //Re-Added by Samer Hamdar, March 23 (Added March 13), 2007: Save Last Record Instead of the First
     
     if ((Vehicles[NumberOfVehicles].VehFrames[i - 1].PreVehID != Vehicles[NumberOfVehicles].VehFrames[i].PreVehID))//Added by Samer Hamdar, March 23, 2007
     {
     LeadersChangingLanestmp = LeadersChangingLanestmp + 1;//Added by Samer Hamdar, March 23, 2007
     }
     Vehicles[NumberOfVehicles].VehFrames[i].NumberOfLaneChanges_D = LeadersChangingLanestmp;//Added by Samer Hamdar: 16 October 2008
     
     
     NumberOfFrames_Relative = NumberOfFrames_Relative + 1;//Added by Samer Hamdar: 20 September 2008
     
     //Conditions 1 for Ending Episode: Changing Lanes
     if ((Vehicles[NumberOfVehicles].VehFrames[i-1].Lane != Vehicles[NumberOfVehicles].VehFrames[i].Lane)&&(i!=Vehicles[NumberOfVehicles].NumFrames-1)) //Modified by Samer Hamdar, March 23, 2007
     {
     //if (veh_data.VehFrames[i-1].SpaceHeadway <= LimitingDistance)
     if (Vehicles[NumberOfVehicles].VehFrames[i - 1].Mode == 1)//Modified by Samer Hamdar: 20 September 2008
     {
     veh_duration_data.EpisodeType = 1;
     }
     else
     {
     veh_duration_data.EpisodeType = 3;
     }
     
     //duration data
     DurationIDtmp = DurationIDtmp + 1;
     
     //add to duration data
     veh_duration_data.DurationID = DurationIDtmp;
     veh_duration_data.DurationTime = DurationTimetmp;
     
     veh_duration_data.EpisodeType = 1;
     veh_duration_data.LeadersChangingLanes = LeadersChangingLanestmp; //Added by Samer Hamdar, March 23, 2007
     
     //Re-Added by Samer Hamdar, March 23 (Removed March 13), 2007 (Save Last Record, Not First)
     veh_duration_data.VehicleType = VehicleClasstmp;
     veh_duration_data.DriverSpeed = VehicleVelocitytmp;
     veh_duration_data.DistanceHeadwayLeader1 = SpaceHeadwaytmp;
     //End of Re-Added by Samer Hamdar, March 23 (Removed March 13), 2007 (Save Last Record, Not First)
     
     //Mofified by Samer Hamdar, March 24, 2007
     //veh_duration_data.CorrespondingFrameID = veh_frame_data.FrameGlobalID;
     //veh_duration_data.CorrespondingFrameID = FrameGlobalIDtmp;
     veh_duration_data.CorrespondingFrameID = Vehicles[NumberOfVehicles].VehFrames[i-1].FrameGlobalID;
     //End of Modified by Samer Hamdar, March 24, 2007
     
     //Recognize the First Episode: Right Censured. Added by Samer Hamdar March 23, 2007
     if (FirstEpisodeDummytmp == 1)
     {
     veh_duration_data.FirstEpisodeDummy = 1;
     }
     else
     {
     veh_duration_data.FirstEpisodeDummy = 0;
     }
     
     veh_duration_data.VehIDDuration = Vehicles[NumberOfVehicles].VehID; //For Time Averaging Recognition: Added by Samer Hamdar, April 12, 2007 (21 September)
     
     //Added by Samer Hamdar: 20 September 2008
     if (Vehicles[NumberOfVehicles].VehFrames[i - 1].Lane == 7)
     veh_duration_data.RampDummy = 1;
     else
     veh_duration_data.RampDummy = 0;
     
     //if (veh_data.VehFrames[i - 1].Lane == 6 || veh_data.VehFrames[i - 1].Lane == 9)
     if (Vehicles[NumberOfVehicles].VehFrames[i - 1].Lane == 6)
     veh_duration_data.RightLaneDummy = 1;
     else
     veh_duration_data.RightLaneDummy = 0;
     
     if (Vehicles[NumberOfVehicles].VehFrames[i - 1].Lane == 1)
     veh_duration_data.LeftLaneDummy = 1;
     else
     veh_duration_data.LeftLaneDummy = 0;
     
     veh_duration_data.DistanceToRamp = fabs (RampLength - Vehicles[NumberOfVehicles].VehFrames[i - 1].LocalY);
     veh_duration_data.DistancetoEntrance = Vehicles[NumberOfVehicles].VehFrames[i - 1].LocalY;
     //End of Added by Samer Hamdar: 20 September 2008
     
     //Added by Samer Hamdar: 21 September 2008
     //veh_duration_data.N_Period = int ( veh_duration_data.DurationTime / veh_data.RT );
     veh_duration_data.N_Period = int ( veh_duration_data.DurationTime / Duration_Disaggregation );
     veh_duration_data.N_Period_Permanent = int ( veh_duration_data.DurationTime / Duration_Disaggregation );
     //End of Added by Samer Hamdar: 21 September 2008
     
     Vehicles[NumberOfVehicles].VehDurations.push_back(veh_duration_data);
     FirstEpisodeDummytmp = FirstEpisodeDummytmp + 1;//Added by Samer Hamdar, March 23, 2007
     Vehicles[NumberOfVehicles].NumDurations = Vehicles[NumberOfVehicles].NumDurations + 1;
     
     //init
     DurationTimetmp = 0;
     LeadersChangingLanestmp = 0; //Added by Samer Hamdar, March 23, 2007
     Vehicles[NumberOfVehicles].VehFrames[i].NumberOfLaneChanges_D = LeadersChangingLanestmp;//Added by Samer Hamdar: 16 October 2008
     }
     
     //Added by Samer Hamdar: March 23, 2007
     //Condition 2 for Ending Episode: Entering from Free-Flow Mode to Car-Following Mode
     //if ((veh_data.VehFrames[i-1].SpaceHeadway > LimitingDistance) && (veh_data.VehFrames[i].SpaceHeadway <= LimitingDistance)&&(i!=veh_data.NumFrames-1))
     if ((Vehicles[NumberOfVehicles].VehFrames[i-1].Mode == 0) && (Vehicles[NumberOfVehicles].VehFrames[i].Mode == 1) && (i!=Vehicles[NumberOfVehicles].NumFrames-1))//Modfied by Samer Hamdar: 20 September 2008
     {
     veh_duration_data.EpisodeType = 4;
     
     //duration data
     DurationIDtmp = DurationIDtmp + 1;
     
     //add to duration data
     veh_duration_data.DurationID = DurationIDtmp;
     veh_duration_data.DurationTime = DurationTimetmp;
     
     veh_duration_data.LeadersChangingLanes = LeadersChangingLanestmp; //Added by Samer Hamdar, March 23, 2007
     
     //Re-Added by Samer Hamdar, March 23 (Removed March 13), 2007 (Save Last Record, Not First)
     veh_duration_data.VehicleType = VehicleClasstmp;
     veh_duration_data.DriverSpeed = VehicleVelocitytmp;
     veh_duration_data.DistanceHeadwayLeader1 = SpaceHeadwaytmp;
     //End of Re-Added by Samer Hamdar, March 23 (Removed March 13), 2007 (Save Last Record, Not First)
     
     //Mofified by Samer Hamdar, March 24, 2007
     //veh_duration_data.CorrespondingFrameID = veh_frame_data.FrameGlobalID;
     //veh_duration_data.CorrespondingFrameID = FrameGlobalIDtmp;
     veh_duration_data.CorrespondingFrameID = Vehicles[NumberOfVehicles].VehFrames[i-1].FrameGlobalID;
     //End of Modified by Samer Hamdar, March 24, 2007
     
     //Recognize the First Episode: Right Censured. Added by Samer Hamdar March 23, 2007
     if (FirstEpisodeDummytmp == 1)
     {
     veh_duration_data.FirstEpisodeDummy = 1;
     }
     else
     {
     veh_duration_data.FirstEpisodeDummy = 0;
     }
     
     veh_duration_data.VehIDDuration = Vehicles[NumberOfVehicles].VehID; //For Time Averaging Recognition: Added by Samer Hamdar, April 12, 2007 (21 September 2008)
     
     //Added by Samer Hamdar: 20 September 2008
     if (Vehicles[NumberOfVehicles].VehFrames[i - 1].Lane == 7)
     veh_duration_data.RampDummy = 1;
     else
     veh_duration_data.RampDummy = 0;
     
     //if (veh_data.VehFrames[i - 1].Lane == 6 || veh_data.VehFrames[i - 1].Lane == 9)
     if (Vehicles[NumberOfVehicles].VehFrames[i - 1].Lane == 6)
     veh_duration_data.RightLaneDummy = 1;
     else
     veh_duration_data.RightLaneDummy = 0;
     
     if (Vehicles[NumberOfVehicles].VehFrames[i - 1].Lane == 1)
     veh_duration_data.LeftLaneDummy = 1;
     else
     veh_duration_data.LeftLaneDummy = 0;
     
     veh_duration_data.DistanceToRamp = fabs (RampLength - Vehicles[NumberOfVehicles].VehFrames[i - 1].LocalY);
     veh_duration_data.DistancetoEntrance = Vehicles[NumberOfVehicles].VehFrames[i - 1].LocalY;
     //End of Added by Samer Hamdar: 20 September 2008
     
     //Added by Samer Hamdar: 21 September 2008
     //veh_duration_data.N_Period = int ( veh_duration_data.DurationTime / veh_data.RT );
     veh_duration_data.N_Period = int ( veh_duration_data.DurationTime / Duration_Disaggregation );
     veh_duration_data.N_Period_Permanent = int ( veh_duration_data.DurationTime / Duration_Disaggregation );
     //End of Added by Samer Hamdar: 21 September 2008
     
     Vehicles[NumberOfVehicles].VehDurations.push_back(veh_duration_data);
     FirstEpisodeDummytmp = FirstEpisodeDummytmp + 1;//Added by Samer Hamdar, March 23, 2007
     Vehicles[NumberOfVehicles].NumDurations = Vehicles[NumberOfVehicles].NumDurations + 1;
     
     //init
     DurationTimetmp = 0;
     LeadersChangingLanestmp = 0; //Added by Samer Hamdar, March 23, 2007
     Vehicles[NumberOfVehicles].VehFrames[i].NumberOfLaneChanges_D = LeadersChangingLanestmp;//Added by Samer Hamdar: 16 October 2008
     }
     
     //Condition 3 for Ending Episode: Entering from Car-Following Model to Free-Flow Mode
     //if ((veh_data.VehFrames[i-1].SpaceHeadway <= LimitingDistance) && (veh_data.VehFrames[i].SpaceHeadway > LimitingDistance)&&(i!=veh_data.NumFrames-1))
     if ((Vehicles[NumberOfVehicles].VehFrames[i-1].Mode == 1) && (Vehicles[NumberOfVehicles].VehFrames[i].Mode == 0)&&(i!=Vehicles[NumberOfVehicles].NumFrames-1))//Modified by Samer Hamdar: 20 September 2008
     {
     veh_duration_data.EpisodeType = 2;
     
     //duration data
     DurationIDtmp = DurationIDtmp + 1;
     
     //add to duration data
     veh_duration_data.DurationID = DurationIDtmp;
     veh_duration_data.DurationTime = DurationTimetmp;
     
     veh_duration_data.LeadersChangingLanes = LeadersChangingLanestmp; //Added by Samer Hamdar, March 23, 2007
     
     //Re-Added by Samer Hamdar, March 23 (Removed March 13), 2007 (Save Last Record, Not First)
     veh_duration_data.VehicleType = VehicleClasstmp;
     veh_duration_data.DriverSpeed = VehicleVelocitytmp;
     veh_duration_data.DistanceHeadwayLeader1 = SpaceHeadwaytmp;
     //End of Re-Added by Samer Hamdar, March 23 (Removed March 13), 2007 (Save Last Record, Not First)
     
     //Mofified by Samer Hamdar, March 24, 2007
     //veh_duration_data.CorrespondingFrameID = veh_frame_data.FrameGlobalID;
     //veh_duration_data.CorrespondingFrameID = FrameGlobalIDtmp;
     veh_duration_data.CorrespondingFrameID = Vehicles[NumberOfVehicles].VehFrames[i-1].FrameGlobalID;
     //End of Modified by Samer Hamdar, March 24, 2007
     
     //Recognize the First Episode: Right Censured. Added by Samer Hamdar March 23, 2007
     if (FirstEpisodeDummytmp == 1)
     {
     veh_duration_data.FirstEpisodeDummy = 1;
     }
     else
     {
     veh_duration_data.FirstEpisodeDummy = 0;
     }
     
     veh_duration_data.VehIDDuration = Vehicles[NumberOfVehicles].VehID; //For Time Averaging Recognition: Added by Samer Hamdar, April 12, 2007 (21 September 2008)
     
     //Added by Samer Hamdar: 20 September 2008
     if (Vehicles[NumberOfVehicles].VehFrames[i - 1].Lane == 7)
     veh_duration_data.RampDummy = 1;
     else
     veh_duration_data.RampDummy = 0;
     
     //if (veh_data.VehFrames[i - 1].Lane == 6 || veh_data.VehFrames[i - 1].Lane == 9)
     if (Vehicles[NumberOfVehicles].VehFrames[i - 1].Lane == 6)
     veh_duration_data.RightLaneDummy = 1;
     else
     veh_duration_data.RightLaneDummy = 0;
     
     if (Vehicles[NumberOfVehicles].VehFrames[i - 1].Lane == 1)
     veh_duration_data.LeftLaneDummy = 1;
     else
     veh_duration_data.LeftLaneDummy = 0;
     
     veh_duration_data.DistanceToRamp = fabs (RampLength - Vehicles[NumberOfVehicles].VehFrames[i - 1].LocalY);
     veh_duration_data.DistancetoEntrance = Vehicles[NumberOfVehicles].VehFrames[i - 1].LocalY;
     //End of Added by Samer Hamdar: 20 September 2008
     
     //Added by Samer Hamdar: 21 September 2008
     //veh_duration_data.N_Period = int ( veh_duration_data.DurationTime / veh_data.RT );
     veh_duration_data.N_Period = int ( veh_duration_data.DurationTime / Duration_Disaggregation );
     veh_duration_data.N_Period_Permanent = int ( veh_duration_data.DurationTime / Duration_Disaggregation );
     //End of Added by Samer Hamdar: 21 September 2008
     
     Vehicles[NumberOfVehicles].VehDurations.push_back(veh_duration_data);
     FirstEpisodeDummytmp = FirstEpisodeDummytmp + 1;//Added by Samer Hamdar, March 23, 2007
     Vehicles[NumberOfVehicles].NumDurations = Vehicles[NumberOfVehicles].NumDurations + 1;
     
     //init
     DurationTimetmp = 0;
     LeadersChangingLanestmp = 0; //Added by Samer Hamdar, March 23, 2007
     Vehicles[NumberOfVehicles].VehFrames[i].NumberOfLaneChanges_D = LeadersChangingLanestmp;//Added by Samer Hamdar: 16 October 2008
     }
     
     //Condition 4 for Ending Episode: Leaving Study Area
     if (i==Vehicles[NumberOfVehicles].NumFrames-1)
     {
     //if (veh_data.VehFrames[i-1].SpaceHeadway <= LimitingDistance)
     if (Vehicles[NumberOfVehicles].VehFrames[i-1].Mode == 1) // Modified by Samer Hamdar: 20 September 2008
     {
     veh_duration_data.EpisodeType = 5;
     }
     else
     {
     veh_duration_data.EpisodeType = 6;
     }
     
     //duration data
     DurationIDtmp = DurationIDtmp + 1;
     
     //add to duration data
     veh_duration_data.DurationID = DurationIDtmp;
     veh_duration_data.DurationTime = DurationTimetmp;
     
     veh_duration_data.LeadersChangingLanes = LeadersChangingLanestmp; //Added by Samer Hamdar, March 23, 2007
     
     //Re-Added by Samer Hamdar, March 23 (Removed March 13), 2007 (Save Last Record, Not First)
     veh_duration_data.VehicleType = VehicleClasstmp;
     veh_duration_data.DriverSpeed = VehicleVelocitytmp;
     veh_duration_data.DistanceHeadwayLeader1 = SpaceHeadwaytmp;
     //End of Re-Added by Samer Hamdar, March 23 (Removed March 13), 2007 (Save Last Record, Not First)
     
     //Mofified by Samer Hamdar, March 24, 2007
     veh_duration_data.CorrespondingFrameID = Vehicles[NumberOfVehicles].VehFrames[i-1].FrameGlobalID;
     //veh_duration_data.CorrespondingFrameID = FrameGlobalIDtmp;
     //veh_duration_data.CorrespondingFrameID = veh_data.VehFrames[i-1].FrameGlobalID;
     //End of Modified by Samer Hamdar, March 24, 2007
     
     //Recognize the First Episode: Right Censured. Added by Samer Hamdar March 23, 2007
     if (FirstEpisodeDummytmp == 1)
     {
     veh_duration_data.FirstEpisodeDummy = 1;
     }
     else
     {
     veh_duration_data.FirstEpisodeDummy = 0;
     }
     
     veh_duration_data.VehIDDuration = Vehicles[NumberOfVehicles].VehID; //For Time Averaging Recognition: Added by Samer Hamdar, April 12, 2007 (21 September 2008)
     
     //Added by Samer Hamdar: 20 September 2008
     if (Vehicles[NumberOfVehicles].VehFrames[i - 1].Lane == 7)
     veh_duration_data.RampDummy = 1;
     else
     veh_duration_data.RampDummy = 0;
     
     //if (veh_data.VehFrames[i - 1].Lane == 6 || veh_data.VehFrames[i - 1].Lane == 9)
     if (Vehicles[NumberOfVehicles].VehFrames[i - 1].Lane == 6)
     veh_duration_data.RightLaneDummy = 1;
     else
     veh_duration_data.RightLaneDummy = 0;
     
     if (Vehicles[NumberOfVehicles].VehFrames[i - 1].Lane == 1)
     veh_duration_data.LeftLaneDummy = 1;
     else
     veh_duration_data.LeftLaneDummy = 0;
     
     veh_duration_data.DistanceToRamp = fabs (RampLength - Vehicles[NumberOfVehicles].VehFrames[i - 1].LocalY);
     veh_duration_data.DistancetoEntrance = Vehicles[NumberOfVehicles].VehFrames[i - 1].LocalY;
     //End of Added by Samer Hamdar: 20 September 2008
     
     //Added by Samer Hamdar: 21 September 2008
     //veh_duration_data.N_Period = int ( veh_duration_data.DurationTime / veh_data.RT );
     veh_duration_data.N_Period = int ( veh_duration_data.DurationTime / Duration_Disaggregation );
     veh_duration_data.N_Period_Permanent = int ( veh_duration_data.DurationTime / Duration_Disaggregation );
     //End of Added by Samer Hamdar: 21 September 2008
     
     Vehicles[NumberOfVehicles].VehDurations.push_back(veh_duration_data);
     FirstEpisodeDummytmp = FirstEpisodeDummytmp + 1;//Added by Samer Hamdar, March 23, 2007
     Vehicles[NumberOfVehicles].NumDurations = Vehicles[NumberOfVehicles].NumDurations + 1;
     
     //init
     DurationTimetmp = 0;
     LeadersChangingLanestmp = 0; //Added by Samer Hamdar, March 23, 2007
     Vehicles[NumberOfVehicles].VehFrames[i].NumberOfLaneChanges_D = LeadersChangingLanestmp;//Added by Samer Hamdar: 16 October 2008
     }
     //Enf of Added by Samer Hamdar, March 23, 2003
     }
     NumberOfVehicles = NumberOfVehicles + 1;//Added by Samer Hamdar, March 18, 2007
     FirstEpisodeDummytmp = 1; //Added by Samer Hamdar, March 23, 2007
     }
     //End of Added by Samer Hamdar: 23 September 2008
     
     //___________Testing Output
     for (int k = 0; k < NumberOfVehicles - 2; k++)
     {
     if ( Vehicles[k].veh_parameter_data_best_mixed_S.MixedError_Velocity < Error_Test_Threshold )
     {
     for (int j = 0; j <Vehicles[k].NumDurations; j++)
     {
     if ( Vehicles[k].VehDurations[j].FirstEpisodeDummy == 0 )
     {
     if (Vehicles[k].VehDurations[j].RightLaneDummy == 1)//Right Lane
     {
     if ( Vehicles[k].VehDurations[j].DistancetoEntrance <=91 && Vehicles[k].VehDurations[j].DistancetoEntrance >= 215 )
     {
     if (Vehicles[k].VehDurations[j].EpisodeType == 1)
     fprintf(out_test_cf1, "%d    %d    %d    %d    %f    %f    %d    %d    %d    %d    %d    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].DurationID, Vehicles[k].VehDurations[j].DurationTime, Vehicles[k].VehDurations[j].EpisodeType, Vehicles[k].VehDurations[j].DriverSpeed, Vehicles[k].VehDurations[j].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].LeadersChangingLanes, Vehicles[k].VehDurations[j].FirstEpisodeDummy, Vehicles[k].VehDurations[j].RightLaneDummy, Vehicles[k].VehDurations[j].LeftLaneDummy, Vehicles[k].VehDurations[j].RampDummy, Vehicles[k].VehDurations[j].DistancetoEntrance, Vehicles[k].VehDurations[j].DistanceToRamp);
     if (Vehicles[k].VehDurations[j].EpisodeType == 2)
     fprintf(out_test_cf2, "%d    %d    %d    %d    %f    %f    %d    %d    %d    %d    %d    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].DurationID, Vehicles[k].VehDurations[j].DurationTime, Vehicles[k].VehDurations[j].EpisodeType, Vehicles[k].VehDurations[j].DriverSpeed, Vehicles[k].VehDurations[j].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].LeadersChangingLanes, Vehicles[k].VehDurations[j].FirstEpisodeDummy, Vehicles[k].VehDurations[j].RightLaneDummy, Vehicles[k].VehDurations[j].LeftLaneDummy, Vehicles[k].VehDurations[j].RampDummy, Vehicles[k].VehDurations[j].DistancetoEntrance, Vehicles[k].VehDurations[j].DistanceToRamp);
     if (Vehicles[k].VehDurations[j].EpisodeType == 3)
     fprintf(out_test_ff3, "%d    %d    %d    %d    %f    %f    %d    %d    %d    %d    %d    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].DurationID, Vehicles[k].VehDurations[j].DurationTime, Vehicles[k].VehDurations[j].EpisodeType, Vehicles[k].VehDurations[j].DriverSpeed, Vehicles[k].VehDurations[j].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].LeadersChangingLanes, Vehicles[k].VehDurations[j].FirstEpisodeDummy, Vehicles[k].VehDurations[j].RightLaneDummy, Vehicles[k].VehDurations[j].LeftLaneDummy, Vehicles[k].VehDurations[j].RampDummy, Vehicles[k].VehDurations[j].DistancetoEntrance, Vehicles[k].VehDurations[j].DistanceToRamp);
     if (Vehicles[k].VehDurations[j].EpisodeType == 4)
     fprintf(out_test_ff4, "%d    %d    %d    %d    %f    %f    %d    %d    %d    %d    %d    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].DurationID, Vehicles[k].VehDurations[j].DurationTime, Vehicles[k].VehDurations[j].EpisodeType, Vehicles[k].VehDurations[j].DriverSpeed, Vehicles[k].VehDurations[j].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].LeadersChangingLanes, Vehicles[k].VehDurations[j].FirstEpisodeDummy, Vehicles[k].VehDurations[j].RightLaneDummy, Vehicles[k].VehDurations[j].LeftLaneDummy, Vehicles[k].VehDurations[j].RampDummy, Vehicles[k].VehDurations[j].DistancetoEntrance, Vehicles[k].VehDurations[j].DistanceToRamp);
     }
     }
     else//Not in the Right Lane
     {
     if (Vehicles[k].VehDurations[j].EpisodeType == 1)
     fprintf(out_test_cf1, "%d    %d    %d    %d    %f    %f    %d    %d    %d    %d    %d    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].DurationID, Vehicles[k].VehDurations[j].DurationTime, Vehicles[k].VehDurations[j].EpisodeType, Vehicles[k].VehDurations[j].DriverSpeed, Vehicles[k].VehDurations[j].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].LeadersChangingLanes, Vehicles[k].VehDurations[j].FirstEpisodeDummy, Vehicles[k].VehDurations[j].RightLaneDummy, Vehicles[k].VehDurations[j].LeftLaneDummy, Vehicles[k].VehDurations[j].RampDummy, Vehicles[k].VehDurations[j].DistancetoEntrance, Vehicles[k].VehDurations[j].DistanceToRamp);
     if (Vehicles[k].VehDurations[j].EpisodeType == 2)
     fprintf(out_test_cf2, "%d    %d    %d    %d    %f    %f    %d    %d    %d    %d    %d    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].DurationID, Vehicles[k].VehDurations[j].DurationTime, Vehicles[k].VehDurations[j].EpisodeType, Vehicles[k].VehDurations[j].DriverSpeed, Vehicles[k].VehDurations[j].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].LeadersChangingLanes, Vehicles[k].VehDurations[j].FirstEpisodeDummy, Vehicles[k].VehDurations[j].RightLaneDummy, Vehicles[k].VehDurations[j].LeftLaneDummy, Vehicles[k].VehDurations[j].RampDummy, Vehicles[k].VehDurations[j].DistancetoEntrance, Vehicles[k].VehDurations[j].DistanceToRamp);
     if (Vehicles[k].VehDurations[j].EpisodeType == 3)
     fprintf(out_test_ff3, "%d    %d    %d    %d    %f    %f    %d    %d    %d    %d    %d    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].DurationID, Vehicles[k].VehDurations[j].DurationTime, Vehicles[k].VehDurations[j].EpisodeType, Vehicles[k].VehDurations[j].DriverSpeed, Vehicles[k].VehDurations[j].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].LeadersChangingLanes, Vehicles[k].VehDurations[j].FirstEpisodeDummy, Vehicles[k].VehDurations[j].RightLaneDummy, Vehicles[k].VehDurations[j].LeftLaneDummy, Vehicles[k].VehDurations[j].RampDummy, Vehicles[k].VehDurations[j].DistancetoEntrance, Vehicles[k].VehDurations[j].DistanceToRamp);
     if (Vehicles[k].VehDurations[j].EpisodeType == 4)
     fprintf(out_test_ff4, "%d    %d    %d    %d    %f    %f    %d    %d    %d    %d    %d    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].DurationID, Vehicles[k].VehDurations[j].DurationTime, Vehicles[k].VehDurations[j].EpisodeType, Vehicles[k].VehDurations[j].DriverSpeed, Vehicles[k].VehDurations[j].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].LeadersChangingLanes, Vehicles[k].VehDurations[j].FirstEpisodeDummy, Vehicles[k].VehDurations[j].RightLaneDummy, Vehicles[k].VehDurations[j].LeftLaneDummy, Vehicles[k].VehDurations[j].RampDummy, Vehicles[k].VehDurations[j].DistancetoEntrance, Vehicles[k].VehDurations[j].DistanceToRamp);
     }
     }
     }
     }
     
     }
     
     
     fclose (out_test_cf1);
     fclose (out_test_cf2);
     fclose (out_test_ff3);
     fclose (out_test_ff4);
     
     //Rearrange the Frame Locations in Each Time Slot After Updating the Positions Based on the Wallsten Model
     
     for (int n = 0; n < t; n++)
     {
     for (int i = 0; i < FrameTime[n].NumFramesLocation - 1; i++)
     {
     for (int j = i+1; j > 0; --j)
     {
     if (FrameTime[n].NumFramesLocation > 1 && FrameTime[n].FrameLocation[j].WallstenX < FrameTime[n].FrameLocation[j-1].WallstenX)
     {
     //swap values
     templocation = FrameTime[n].FrameLocation[j];
     FrameTime[n].FrameLocation[j] = FrameTime[n].FrameLocation[j - 1];
     FrameTime[n].FrameLocation[j - 1] = templocation;
     }
     }
     }
     }
     
     
     //________Recompute Duration Characteristics
     AverageSurroundingDensitytmp = 0.0;
     AverageSurroundingDensityLaneRtmp = 0.0;
     AverageSurroundingDensityLaneLtmp = 0.0;
     //End of Added by Samer Hamdar, April 4, 2007 (21 September 2008)
     
     //Modified by Samer Hamdar, March 25, 2007
     k = 0;//to track vehicles data
     j = 0;//to track veh.duration data
     x = 0;//to track frametime data
     y = 0;//to track frametime.framelocation data
     //End of Modified by Samer Hamdar, March 25, 2007
     
     //Added by Samer Hamdar, April 12, 2007 (21 September 2008)
     int tt;//Counter Time for Following Through Average
     int yy; // Counter to Revisit all LocationData in Each Previous Time Step
     //End of Added by Samer Hamdar, April 12, 2007 (21 September 2008)
     
     int NPeriodtmp;
     
     //Added by Samer Hamdar, March 25, 2007
     while (k < NumberOfVehicles - 2) //Go Through All Vehicles
     {
     j = 0;// Initialize j to go through durations corresponding to vehicle k
     
     while (j < Vehicles[k].NumDurations) // Go Through All Durations For Each Vehicle
     {
     x = 0; // Initiate k to go though all FrameTimes when Revisiting new Durations
     
     while (x < t) //Search for the Frame Limiting The Episode in the Sorted Data in Time Level
     {
     y = 0;// Initialize y to go through FrameLocations Corresponding to Frame Time Slot x
     
     while (y < FrameTime[x].NumFramesLocation)//Search for the Frame Limiting The Episode in the Sorted Data in Space Level
     {
     if (FrameTime[x].FrameLocation[y].FrameGlobalID == Vehicles[k].VehDurations[j].CorrespondingFrameID)
     {
     //Time Averaging, Added by Samer Hamdar, April 12, 2007 (21 September 2008)
     //tt = 0;//Modified
     tt = Vehicles[k].VehDurations[j].N_Period;
     
     //Added by Samer Hamdar, April 12, 2007
     DriverSpeedtmp = 0.0;             //At the end of the duration (temporary)
     RelativeSpeedLeader1tmp = 0.0;    //At the end of the duration (temporary) (999 if no leader 1)
     DistanceHeadwayLeader1tmp = 0.0;  //At the end of the duration (temporary) (9999 if no leader 1) (front to front)
     RelativeSpeedFollower1tmp = 0.0;  //At the end of the duration (temporary) (999 if no follower 1)
     DistanceHeadwayFollower1tmp = 0.0;//At the end of the duration (temporary) (9999 if no follower 1) (front to front)
     RelativeSpeedLeader2tmp = 0.0;    //At the end of the duration (temporary) (999 if no leader 2)
     DistanceHeadwayLeader2tmp =0.0;  //At the end of the duration (temporary) (9999 if no leader 2) (front to front)
     
     AverageSurroundingDensitytmpMajor = 0.0; //At the end of the duration (temporary)
     AverageSurroundingDensityLaneLtmpMajor = 0.0; //At the end of the duration (temporary) (If no left lane: 9999.99)
     AverageSurroundingDensityLaneRtmpMajor = 0.0; //At the end of the duration (temporary) (If no right lane: 9999.99)
     DistancetoEntrancetmp = 0.0; //At the end of the duration (temporary) (If no entrance: 999.99)
     
     RelativeSpeedLeftL1tmp = 0.0;// With the Leader at the Target Lane to The Left (999 if No Leader Available)
     DistanceHeadwayLeftL1tmp = 0.0;// 9999 if not available
     RelativeSpeedRightL1tmp = 0.0;// With the Leader at the Target Lane to The Right (999 if No Leader Available)
     DistanceHeadwayRightL1tmp = 0.0;// 9999 if not available
     RelativeSpeedLeftF1tmp = 0.0;// With the Follower at the Target Lane to The Left (999 if No Leader Available)
     DistanceHeadwayLeftF1tmp = 0.0;// 9999 if not available
     RelativeSpeedRightF1tmp = 0.0;// With the Follower at the Target Lane to The Right (999 if No Leader Available)
     DistanceHeadwayRightF1tmp = 0.0;// 9999 if not available
     
     NPeriodtmp = 0;
     
     //_____________________________________________________________________________________________________________________________________________________________________
     //Added by Samer Hamdar: 21 September 2008
     while (tt >= 0)
     {
     yy = 0;
     
     while (yy < FrameTime[x - ( tt * Duration_Disaggregation )].NumFramesLocation)
     {
     if (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].VehIDFrame ==  Vehicles[k].VehDurations[j].VehIDDuration)
     {
     
     VehDurationDataSUB veh_duration_data_SUB;
     
     veh_duration_data_SUB.LeadersChangingLanes = FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].LeadersChangingLanes_Frame;
     
     //Speed
     veh_duration_data_SUB.DriverSpeed = FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenVelocity;
     
     //Distance_To_Ramp
     veh_duration_data_SUB.DistanceToRamp = fabs ( RampLength - ( FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX ));
     
     
     //First Leader Related Data (Same Lane)
     s = FrameTime[x - ( tt * Duration_Disaggregation )].NumFramesLocation - yy;
     
     dummy = 0;
     for (int w = 1; w < s; w++)
     {
     if (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].Lane == FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy+w].Lane)
     {
     dummy = 1;
     veh_duration_data_SUB.RelativeSpeedLeader1 = (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenVelocity - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy+w].WallstenVelocity);
     veh_duration_data_SUB.DistanceHeadwayLeader1 = (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy+w].WallstenX - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX);
     w = s+1;
     }
     }
     if (dummy == 0)//No First Leader
     {
     if ( FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].Lane == 7 )
     {
     veh_duration_data_SUB.RelativeSpeedLeader1 = FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenVelocity;
     veh_duration_data_SUB.DistanceHeadwayLeader1 = ( RampLength - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX );
     }
     else
     {
     veh_duration_data_SUB.RelativeSpeedLeader1 = -1.1234;
     veh_duration_data_SUB.DistanceHeadwayLeader1 = 20.1234;
     }
     }
     
     //Second Leader Related Data (Same Lane)
     dummy = 0;
     track = 2;
     for (int w = 1; w < s; w++)
     {
     if (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].Lane == FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy+w].Lane)
     {
     track --;
     if (track == 0)
     {
     dummy = 1;
     veh_duration_data_SUB.RelativeSpeedLeader2 = (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenVelocity - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy+w].WallstenVelocity);
     veh_duration_data_SUB.DistanceHeadwayLeader2 = (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy+w].WallstenX - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX);
     w = s+1;
     }
     }
     }
     if (dummy == 0)//No Second Leader
     {
     veh_duration_data_SUB.RelativeSpeedLeader2 = -1.1234;
     veh_duration_data_SUB.DistanceHeadwayLeader2 = 35.1234;
     }
     
     //First Follower Related Data (Same Lane)
     dummy = 0;
     for (int w = 1; w <= yy; w++)
     {
     if (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].Lane == FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy-w].Lane)
     {
     dummy = 1;
     veh_duration_data_SUB.RelativeSpeedFollower1 = (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenVelocity - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy-w].WallstenVelocity);
     veh_duration_data_SUB.DistanceHeadwayFollower1 = (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy-w].WallstenX);
     w = yy+1;
     }
     }
     if (dummy == 0)//No First Follower
     {
     veh_duration_data_SUB.RelativeSpeedFollower1 = 1.1234;
     veh_duration_data_SUB.DistanceHeadwayFollower1 = 20.1234;
     }
     
     //First Leader Related Data (Left Lane)
     s = FrameTime[x - ( tt * Duration_Disaggregation )].NumFramesLocation - yy;
     dummy = 0;
     for (int w = 1; w < s; w++)
     {
     if (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].Lane == (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy+w].Lane + 1))
     {
     dummy = 1;
     veh_duration_data_SUB.RelativeSpeedLeftL1 = (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenVelocity - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy+w].WallstenVelocity);
     veh_duration_data_SUB.DistanceHeadwayLeftL1 = (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy+w].WallstenX - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX);
     w = s+1;
     }
     }
     if (dummy == 0)//No First Leader / (Lane 1) (NB: Data related to Lane 9 - shoulder are ignored) + (Lane Seven Always Consider Headway Ahead)
     {
     //veh_duration_data_SUB.RelativeSpeedLeftL1 = 999;
     //veh_duration_data_SUB.DistanceHeadwayLeftL1 = 9999;
     if ( FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].Lane == 1 )
     {
     veh_duration_data_SUB.RelativeSpeedLeftL1 = 30.1234;
     veh_duration_data_SUB.DistanceHeadwayLeftL1 = 0.1234;
     }
     else
     {
     veh_duration_data_SUB.RelativeSpeedLeftL1 = -1.1234;
     veh_duration_data_SUB.DistanceHeadwayLeftL1 = 20.1234;
     }
     }
     
     //First Leader Related Data (Right Lane)
     dummy = 0;
     for (int w = 1; w < s; w++)
     {
     if (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].Lane == (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy+w].Lane - 1) && FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].Lane != 6)
     {
     dummy = 1;
     veh_duration_data_SUB.RelativeSpeedRightL1 = (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenVelocity - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy+w].WallstenVelocity);
     veh_duration_data_SUB.DistanceHeadwayRightL1 = (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy+w].WallstenX - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX);
     w = s+1;
     }
     else if (  FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].Lane == 6  )
     {
     if ( FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX > 91 && FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX < 215)
     {
     dummy = 1;
     veh_duration_data_SUB.RelativeSpeedRightL1 = (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenVelocity - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy+w].WallstenVelocity);
     veh_duration_data_SUB.DistanceHeadwayRightL1 = (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy+w].WallstenX - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX);
     w = s+1;
     }
     else
     {
     dummy = 1;
     veh_duration_data_SUB.RelativeSpeedRightL1 = 30.1234;
     veh_duration_data_SUB.DistanceHeadwayRightL1 = 0.1234;
     }
     
     }
     }
     if (dummy == 0)//No First Leader / Lane 7/ (NB: Data related to Lane 9 - shoulder are ignored)
     {
     //veh_duration_data_SUB.RelativeSpeedRightL1 = 999;
     //veh_duration_data_SUB.DistanceHeadwayRightL1 = 9999;
     if ( FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].Lane == 7 )
     {
     veh_duration_data_SUB.RelativeSpeedRightL1 = 30.1234;
     veh_duration_data_SUB.DistanceHeadwayRightL1 = 0.1234;
     }
     else
     {
     veh_duration_data_SUB.RelativeSpeedRightL1 = -1.1234;
     veh_duration_data_SUB.DistanceHeadwayRightL1 = 20.1234;
     }
     }
     
     
     //First Follower Related Data (Left Lane)
     dummy = 0;
     for (int w = 1; w <= yy; w++)
     {
     if (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].Lane == (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy-w].Lane + 1))
     {
     dummy = 1;
     veh_duration_data_SUB.RelativeSpeedLeftF1 =(FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenVelocity - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy-w].WallstenVelocity);
     veh_duration_data_SUB.DistanceHeadwayLeftF1 = (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy-w].WallstenX);
     w = yy+1;
     }
     }
     if (dummy == 0)//No First Follower / (Lane 1)
     {
     //veh_duration_data_SUB.RelativeSpeedLeftF1 = 999;
     //veh_duration_data_SUB.DistanceHeadwayLeftF1 = 9999;
     if ( FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].Lane == 1 )
     {
     veh_duration_data_SUB.RelativeSpeedLeftF1 = -30.1234;
     veh_duration_data_SUB.DistanceHeadwayLeftF1 = 0.1234;
     }
     else
     {
     veh_duration_data_SUB.RelativeSpeedLeftF1 = 1.1234;
     veh_duration_data_SUB.DistanceHeadwayLeftF1 = 20.1234;
     }
     }
     
     //First Follower Related Data (Right Lane)
     dummy = 0;
     for (int w = 1; w <= yy; w++)
     {
     if (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].Lane == (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy-w].Lane - 1) && FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].Lane != 6)
     {
     dummy = 1;
     veh_duration_data_SUB.RelativeSpeedRightF1 = (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenVelocity - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy-w].WallstenVelocity);
     veh_duration_data_SUB.DistanceHeadwayRightF1 = (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy-w].WallstenX);
     w = yy+1;
     }
     else if ( FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].Lane == 6 )
     {
     if ( FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX > 91 && FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX < 215)
     {
     dummy = 1;
     veh_duration_data_SUB.RelativeSpeedRightF1 = (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenVelocity - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy-w].WallstenVelocity);
     veh_duration_data_SUB.DistanceHeadwayRightF1 = (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy-w].WallstenX);
     w = yy+1;
     
     }
     else
     {
     dummy = 1;
     veh_duration_data_SUB.RelativeSpeedRightF1 = -30.1234;
     veh_duration_data_SUB.DistanceHeadwayRightF1 = 0.1234;
     }
     }
     }
     if (dummy == 0)//No First Follower / (Lane 7)
     {
     //veh_duration_data_SUB.RelativeSpeedRightF1 = 999;
     //veh_duration_data_SUB.DistanceHeadwayRightF1 = 9999;
     if ( FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].Lane == 7 )
     {
     veh_duration_data_SUB.RelativeSpeedRightF1 = - 30.1234;
     veh_duration_data_SUB.DistanceHeadwayRightF1 = 0.1234;
     }
     else
     {
     veh_duration_data_SUB.RelativeSpeedRightF1 = 1.1234;
     veh_duration_data_SUB.DistanceHeadwayRightF1 = 20.1234;
     }
     }
     
     //Average Density Related Data
     AverageSurroundingDensitytmp = 1;//Modified by Samer Hamdar, April 5, 2007: the vehicle of interest is considered in the surrounding density
     AverageSurroundingDensityLaneRtmp = 0;
     AverageSurroundingDensityLaneLtmp = 0;
     
     //General
     ww = 1;
     //Ahead
     while ( ww < s)
     {
     if ( FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy + ww].WallstenX - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX <= DistanceDensityAhead)
     {
     AverageSurroundingDensitytmp = AverageSurroundingDensitytmp + 1;
     }
     ww = ww + 1;
     }
     
     //Behind
     ww = 1;
     while (ww <= yy)
     {
     if (  FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX  - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy - ww].WallstenX <= DistanceDensityBehind)
     {
     AverageSurroundingDensitytmp = AverageSurroundingDensitytmp + 1;
     }
     ww = ww + 1;
     }
     
     //Interpolation
     if ((FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX < DistanceDensityBehind) && (L - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX >= DistanceDensityAhead))
     {
     AverageSurroundingDensitytmp = (AverageSurroundingDensitytmp * (DistanceDensityAhead + DistanceDensityBehind) / (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX + DistanceDensityAhead));
     }
     
     if ((FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX >= DistanceDensityBehind) && (L - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX < DistanceDensityAhead))
     {
     AverageSurroundingDensitytmp = (AverageSurroundingDensitytmp * (DistanceDensityAhead + DistanceDensityBehind) / (DistanceDensityBehind + L - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX));
     }
     
     if ((FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX < DistanceDensityBehind) && (L - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX < DistanceDensityAhead))
     {
     AverageSurroundingDensitytmp = (AverageSurroundingDensitytmp * (DistanceDensityAhead + DistanceDensityBehind) / L);
     }
     
     AverageSurroundingDensitytmp = ((AverageSurroundingDensitytmp * 1000.0)/(DistanceDensityAhead + DistanceDensityBehind) / NLanes);
     veh_duration_data_SUB.AverageSurroundingDensity = AverageSurroundingDensitytmp;
     
     
     //Average Density to the Right Related Data
     ww = 1;
     //Ahead
     while ( ww < s)
     {
     if ((FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy + ww].WallstenX - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX <= DistanceDensityAhead) && (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].Lane == (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy + ww].Lane - 1)) )
     {
     AverageSurroundingDensityLaneRtmp = AverageSurroundingDensityLaneRtmp + 1;
     }
     ww = ww + 1;
     }
     
     //Behind
     ww = 1;
     while (ww <= yy)
     {
     if (  FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX  - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy - ww].WallstenX <= DistanceDensityBehind && (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].Lane == (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy-ww].Lane - 1)))
     {
     AverageSurroundingDensityLaneRtmp = AverageSurroundingDensityLaneRtmp + 1;
     }
     ww = ww + 1;
     }
     
     //Interpolation
     if ((FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX < DistanceDensityBehind) && (L - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX >= DistanceDensityAhead))
     {
     AverageSurroundingDensityLaneRtmp = (AverageSurroundingDensityLaneRtmp * (DistanceDensityAhead + DistanceDensityBehind) / (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX + DistanceDensityAhead));
     }
     
     if ((FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX >= DistanceDensityBehind) && (L - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX < DistanceDensityAhead))
     {
     AverageSurroundingDensityLaneRtmp = (AverageSurroundingDensityLaneRtmp * (DistanceDensityAhead + DistanceDensityBehind) / (DistanceDensityBehind + L - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX));
     }
     
     if ((FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX < DistanceDensityBehind) && (L - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX < DistanceDensityAhead))
     {
     AverageSurroundingDensityLaneRtmp = (AverageSurroundingDensityLaneRtmp * (DistanceDensityAhead + DistanceDensityBehind) / L);
     }
     
     AverageSurroundingDensityLaneRtmp = ((AverageSurroundingDensityLaneRtmp * 1000.0)/(DistanceDensityAhead + DistanceDensityBehind));// 1 mile = 5280 fts
     
     
     if (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].Lane == 6)
     {
     if ( FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX > 91 && FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX < 215)
     {
     AverageSurroundingDensityLaneRtmp = AverageSurroundingDensityLaneRtmp;
     }
     else
     {
     AverageSurroundingDensityLaneRtmp = 100.1234;
     
     }
     }
     
     if (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].Lane == 7)
     {
     AverageSurroundingDensityLaneRtmp = 100.1234;
     }
     
     veh_duration_data_SUB.AverageSurroundingDensityLaneR = AverageSurroundingDensityLaneRtmp;
     
     //Average Density to the Left Related Data
     ww = 1;
     //Ahead
     while ( ww < s)
     {
     if ((FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy + ww].WallstenX - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX <= DistanceDensityAhead) && (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].Lane == (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy + ww].Lane + 1)) )
     {
     AverageSurroundingDensityLaneLtmp = AverageSurroundingDensityLaneLtmp + 1;
     }
     ww = ww + 1;
     }
     
     //Behind
     ww = 1;
     while (ww <= yy)
     {
     if (  ( FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX  - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX <= DistanceDensityBehind ) && ( FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].Lane == (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy-ww].Lane + 1) ) )
     {
     AverageSurroundingDensityLaneLtmp = AverageSurroundingDensityLaneLtmp + 1;
     }
     ww = ww + 1;
     }
     
     //Interpolation
     if ((FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX < DistanceDensityBehind) && (L - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX >= DistanceDensityAhead))
     {
     AverageSurroundingDensityLaneLtmp = (AverageSurroundingDensityLaneLtmp * (DistanceDensityAhead + DistanceDensityBehind) / (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX + DistanceDensityAhead));
     }
     
     if ((FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX >= DistanceDensityBehind) && (L - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX < DistanceDensityAhead))
     {
     AverageSurroundingDensityLaneLtmp = (AverageSurroundingDensityLaneLtmp * (DistanceDensityAhead + DistanceDensityBehind) / (DistanceDensityBehind + L - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX));
     }
     
     if ((FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX < DistanceDensityBehind) && (L - FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].WallstenX < DistanceDensityAhead))
     {
     AverageSurroundingDensityLaneLtmp = (AverageSurroundingDensityLaneLtmp * (DistanceDensityAhead + DistanceDensityBehind) / L);
     
     }
     
     AverageSurroundingDensityLaneLtmp = ((AverageSurroundingDensityLaneLtmp * 1000.0)/(DistanceDensityAhead + DistanceDensityBehind));// 1 mile = 5280 fts
     
     if (FrameTime[x - ( tt * Duration_Disaggregation )].FrameLocation[yy].Lane == 1)
     {
     AverageSurroundingDensityLaneLtmp = 100.1234;
     }
     
     veh_duration_data_SUB.AverageSurroundingDensityLaneL = AverageSurroundingDensityLaneLtmp;
     
     veh_duration_data_SUB.DurationID = Vehicles[k].VehDurations[j].DurationID;
     veh_duration_data_SUB.DurationTimeSUB = ( Vehicles[k].VehDurations[j].DurationTime - ( tt * Duration_Disaggregation ) );
     veh_duration_data_SUB.EpisodeType = Vehicles[k].VehDurations[j].EpisodeType;
     veh_duration_data_SUB.FirstEpisodeDummy = Vehicles[k].VehDurations[j].FirstEpisodeDummy;
     veh_duration_data_SUB.VehicleType = Vehicles[k].VehDurations[j].VehicleType;
     veh_duration_data_SUB.LeftLaneDummy = Vehicles[k].VehDurations[j].LeftLaneDummy;
     veh_duration_data_SUB.RampDummy = Vehicles[k].VehDurations[j].RampDummy;
     veh_duration_data_SUB.RightLaneDummy = Vehicles[k].VehDurations[j].RightLaneDummy;
     veh_duration_data_SUB.N_Period = ( Vehicles[k].VehDurations[j].N_Period_Permanent + 1 );
     
     Vehicles[k].VehDurations[j].VehDurationsSUB.push_back(veh_duration_data_SUB);
     
     NPeriodtmp = NPeriodtmp + 1;
     
     }
     yy++;
     }
     tt--;
     }
     }
     y++;
     }
     x++;
     }
     j++;
     }
     k++;
     }
     //End of Added by Samer Hamdar, March 25, 2007
     
     
     ////Added by Samer Hamdar: 21 September 2008
     //k = 0;
     
     //while (k < NumberOfVehicles) //Go Through All Vehicles
     //{
     //    j = 0;// Initialize j to go through durations corresponding to vehicle k
     
     //    while (j < Vehicles[k].NumDurations) // Go Through All Durations For Each Vehicle
     //    {
     //        for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     //        {
     //            //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     //            fprintf(out, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period - 1 ), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //            //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     //        }
     //        j++;
     //    }
     //    k++;
     //}
     ////End of Added by Samer Hamdar: 21 September 2008
     
     //Added by Samer Hamdar: 22 September 2008
     //NumberOfVehicles = 13;
     
     for (int k = 0; k < NumberOfVehicles - 2;k++)//NumberOfVehicles - 2; k++) //Go Through All Vehicles
     {
     if ( Vehicles[k].veh_parameter_data_best_mixed_S.MixedError_Velocity < Error_Test_Threshold )
     {
     for ( int j = 0; j < Vehicles[k].NumDurations; j++) // Go Through All Durations For Each Vehicle
     {
     if (Vehicles[k].VehDurations[j].DurationTime >= 10 )
     {
     
     
     
     if ( Vehicles[k].VehDurations[j].EpisodeType != 5 && Vehicles[k].VehDurations[j].EpisodeType != 6 )//Without Types 5 and 6
     {
     if (Vehicles[k].VehDurations[j].RightLaneDummy == 1)//Right Lane
     {
     if ( Vehicles[k].VehDurations[j].DistancetoEntrance <=91 && Vehicles[k].VehDurations[j].DistancetoEntrance >= 215 )
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     else if ( Vehicles[k].VehDurations[j].DistancetoEntrance > 91 && Vehicles[k].VehDurations[j].DistancetoEntrance < 215 )
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out_R, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     
     }
     else
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     
     }
     }
     
     //________________________________________________________________________
     
     //All Lanes
     if (Vehicles[k].VehDurations[j].EpisodeType == 1)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out1, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     
     if (Vehicles[k].VehDurations[j].EpisodeType == 2)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out2, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     
     if (Vehicles[k].VehDurations[j].EpisodeType == 3)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out3, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     
     if (Vehicles[k].VehDurations[j].EpisodeType == 4)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out4, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     
     if (Vehicles[k].VehDurations[j].EpisodeType == 5)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out5, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     
     if (Vehicles[k].VehDurations[j].EpisodeType == 6)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out6, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     
     
     //Ramp Lane
     if (Vehicles[k].VehDurations[j].RampDummy == 1)
     {
     if (Vehicles[k].VehDurations[j].EpisodeType == 1)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out7_1, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     
     if (Vehicles[k].VehDurations[j].EpisodeType == 2)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out7_2, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     
     if (Vehicles[k].VehDurations[j].EpisodeType == 3)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out7_3, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     
     if (Vehicles[k].VehDurations[j].EpisodeType == 4)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out7_4, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     
     if (Vehicles[k].VehDurations[j].EpisodeType == 5)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out7_5, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     
     if (Vehicles[k].VehDurations[j].EpisodeType == 6)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out7_6, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     }
     
     
     //Right Lane Far from On-Ramp
     if (Vehicles[k].VehDurations[j].RightLaneDummy == 1)
     {
     if ( Vehicles[k].VehDurations[j].DistancetoEntrance <=91 && Vehicles[k].VehDurations[j].DistancetoEntrance >= 215 )
     {
     if (Vehicles[k].VehDurations[j].EpisodeType == 1)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out8_1, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     
     if (Vehicles[k].VehDurations[j].EpisodeType == 2)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out8_2, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     
     if (Vehicles[k].VehDurations[j].EpisodeType == 3)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out8_3, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     
     if (Vehicles[k].VehDurations[j].EpisodeType == 4)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out8_4, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     
     if (Vehicles[k].VehDurations[j].EpisodeType == 5)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out8_5, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     
     if (Vehicles[k].VehDurations[j].EpisodeType == 6)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out8_6, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     }
     }
     
     //Right Lane Close To On-Ramp
     if (Vehicles[k].VehDurations[j].RightLaneDummy == 1)
     {
     if ( Vehicles[k].VehDurations[j].DistancetoEntrance > 91 && Vehicles[k].VehDurations[j].DistancetoEntrance < 215 )
     {
     if (Vehicles[k].VehDurations[j].EpisodeType == 1)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out8_1R, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     
     if (Vehicles[k].VehDurations[j].EpisodeType == 2)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out8_2R, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     
     if (Vehicles[k].VehDurations[j].EpisodeType == 3)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out8_3R, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     
     if (Vehicles[k].VehDurations[j].EpisodeType == 4)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out8_4R, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     
     if (Vehicles[k].VehDurations[j].EpisodeType == 5)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out8_5R, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     
     if (Vehicles[k].VehDurations[j].EpisodeType == 6)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out8_6R, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     }
     }
     
     //Left Lane
     if ( Vehicles[k].VehDurations[j].LeftLaneDummy == 1)
     {
     if (Vehicles[k].VehDurations[j].EpisodeType == 1)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out9_1, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     
     if (Vehicles[k].VehDurations[j].EpisodeType == 2)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out9_2, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     
     if (Vehicles[k].VehDurations[j].EpisodeType == 3)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out9_3, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     
     if (Vehicles[k].VehDurations[j].EpisodeType == 4)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out9_4, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     
     if (Vehicles[k].VehDurations[j].EpisodeType == 5)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out9_5, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     
     if (Vehicles[k].VehDurations[j].EpisodeType == 6)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out9_6, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     }
     
     //Middle Lanes
     if ( Vehicles[k].VehDurations[j].LeftLaneDummy == 0 && Vehicles[k].VehDurations[j].RightLaneDummy == 0 && Vehicles[k].VehDurations[j].RampDummy == 0)
     {
     if (Vehicles[k].VehDurations[j].EpisodeType == 1)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out10_1, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     
     if (Vehicles[k].VehDurations[j].EpisodeType == 2)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out10_2, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     
     if (Vehicles[k].VehDurations[j].EpisodeType == 3)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out10_3, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     
     if (Vehicles[k].VehDurations[j].EpisodeType == 4)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out10_4, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     
     if (Vehicles[k].VehDurations[j].EpisodeType == 5)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out10_5, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     
     if (Vehicles[k].VehDurations[j].EpisodeType == 6)
     {
     for (int NP = 0; NP < Vehicles[k].VehDurations[j].N_Period; NP ++ )
     {
     //Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     fprintf(out10_6, "%d    %d    %d    %d    %d    %d    %d    %d    %d    %f    %d    %d    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",Vehicles[k].VehID, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationID, ( Vehicles[k].VehDurations[j].VehDurationsSUB[NP].N_Period), Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DurationTimeSUB, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].EpisodeType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].FirstEpisodeDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RightLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeftLaneDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RampDummy, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceToRamp, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].VehicleType, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].LeadersChangingLanes, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DriverSpeed, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedFollower1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeader2, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedRightF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftL1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].DistanceHeadwayLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].RelativeSpeedLeftF1, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensity, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneR, Vehicles[k].VehDurations[j].VehDurationsSUB[NP].AverageSurroundingDensityLaneL);
     //End of Re-Modified by Samer Hamdar, March 26, 2007 (21 September 2008)
     }
     }
     }
     }
     //___________________________________________________________________________
     }
     }
     }
     //End of Added by Samer Hamdar: 22 September 2008
     
     */
    //End of Added by Samer Hamdar: 15 October 2008
    //***************************End of Re-retrieval of Duration Lengths/Types/Characteristics*************************
    
    
    //*********************** Create Output Files ******************************************************************************************
    
    /*
     //Sample Trajectory Measure Comparison
     for (int n = 0; n < ( 13 - 2 ) ; n++)//NumOfVehicles is over-estimated by 2
     {
     if (n < NumberOfVehicles - 2 )
     {
     fprintf(out1_w, "%f    %f    %f    %f    %f    %f    %f    %d    %f    %f\n", Vehicles[n].veh_parameter_data_best_mixed_S.Gamma, Vehicles[n].veh_parameter_data_best_mixed_S.Wm, Vehicles[n].veh_parameter_data_best_mixed_S.Wc, Vehicles[n].veh_parameter_data_best_mixed_S.Tmax, Vehicles[n].veh_parameter_data_best_mixed_S.Alpha, Vehicles[n].veh_parameter_data_best_mixed_S.Beta, Vehicles[n].veh_parameter_data_best_mixed_S.Tcorr, Vehicles[n].veh_parameter_data_best_mixed_S.RT, Vehicles[n].veh_parameter_data_best_mixed_S.MixedError_Gap, Vehicles[n].veh_parameter_data_best_mixed_S.MixedError_Velocity);
     
     for (int f = 0; f < Vehicles[n].NumFrames - 2 ; f++) //Th
     {
     if ( Vehicles[n].VehFrames[f].PreVehID  != 0)
     {
     fprintf(out1_w, "%d    %d    %f    %f    %f    %f    %f    %f    %f    %f\n", Vehicles[n].VehID, Vehicles[n].VehFrames[f].FrameID, Vehicles[n].VehFrames[f+1].VehicleAcceleration, Vehicles[n].VehFrames[f].PermanentWallstenAcceleration_S, Vehicles[n].VehFrames[f+1].WallstenVelocity, Vehicles[n].VehFrames[f].PermanentWallstenVelocity_S, Vehicles[n].VehFrames[f+1].SpaceGap,Vehicles[n].VehFrames[f].PermanentWallstenGap_S, Vehicles[n].VehFrames[f+1].RelativeVelocity, Vehicles[n].VehFrames[f].PermanentWallstenRelativeSpeed_S );
     }
     else
     {
     fprintf(out1_w, "%d    %d    %f    %f    %f    %f    %f    %f    %f    %f\n", Vehicles[n].VehID, Vehicles[n].VehFrames[f].FrameID, Vehicles[n].VehFrames[f+1].VehicleAcceleration, Vehicles[n].VehFrames[f].DummyWallstenMeasure, Vehicles[n].VehFrames[f+1].WallstenVelocity, Vehicles[n].VehFrames[f].DummyWallstenMeasure, Vehicles[n].VehFrames[f+1].SpaceGap, Vehicles[n].VehFrames[f].DummyWallstenMeasure, Vehicles[n].VehFrames[f+1].RelativeVelocity, -20.0);
     }
     }
     }
     }
     */
    
    
    
    
    //Vehicle Specific Calibrated parameters with Corresponding Error
    //for (int n = 0; n < NumberOfVehicles - 2; n++)//NumOfVehicles is over-estimated by 2
    for (int n = 0; n < number_of_cars_to_be_analyzed; n++)//NumOfVehicles is over-estimated by 2
    {
        fprintf(out2_w, "%f    %f    %f    %f    %f    %f    %f    %f    %f    %d    %f    %f    %f    %f\n",  Vehicles[n].veh_parameter_data_best_mixed_S.PSI, Vehicles[n].veh_parameter_data_best_mixed_S.Gamma1, Vehicles[n].veh_parameter_data_best_mixed_S.Gamma2, Vehicles[n].veh_parameter_data_best_mixed_S.Wm, Vehicles[n].veh_parameter_data_best_mixed_S.Wc, Vehicles[n].veh_parameter_data_best_mixed_S.Tmax, Vehicles[n].veh_parameter_data_best_mixed_S.Alpha, Vehicles[n].veh_parameter_data_best_mixed_S.Beta, Vehicles[n].veh_parameter_data_best_mixed_S.Tcorr, Vehicles[n].veh_parameter_data_best_mixed_S.RT, Vehicles[n].veh_parameter_data_best_mixed_S.MixedError_Gap, Vehicles[n].veh_parameter_data_best_mixed_S.MixedError_Velocity,
            Vehicles[n].veh_parameter_data_best_mixed_S.MixedError_Gap_Validation, Vehicles[n].veh_parameter_data_best_mixed_S.MixedError_Velocity_Validation);
    }
    
    /*
     for (int kk = 0; kk < NumberOfVehicles - 2; kk++)
     {
     for (int ff = 0; ff < Vehicles[kk].NumberOfLaneChanges - 1; ff++)
     {
     fprintf (out4_w, "%d    %f    %f    %d\n", Vehicles[kk].LaneChanging[ff].LaneChangingTime, Vehicles[kk].LaneChanging[ff].LaneChangingHazard, Vehicles[kk].LaneChanging[ff].CompetingHazard, Vehicles[kk].LaneChanging[ff].Mode);
     }
     }
     */
    //    struct LaneChangingData
    //{
    //    int LaneChangingTime;
    //    double LaneChangingHazard;
    //    double CompetingHazard;
    //    int Mode;
    //};
    
    //Sensitivity Analysis Measures
    //.
    //.
    //.
    
    
    ////Output GA - Space Optimization Measure: at Each Time Step for Velocity, Gap and Relative Velocity - Relative Error
    //for (int n = 0; n < 1 ; n++)//NumOfVehicles is over-estimated by 2
    //{
    //    fprintf(out1_w, "%f    %f    %f    %f    %f    %f    %f    %d    %f\n\n", Vehicles[n].veh_parameter_data_best_relative_S.Gamma, Vehicles[n].veh_parameter_data_best_relative_S.Wm, Vehicles[n].veh_parameter_data_best_relative_S.Wc, Vehicles[n].veh_parameter_data_best_relative_S.Tmax, Vehicles[n].veh_parameter_data_best_relative_S.Alpha, Vehicles[n].veh_parameter_data_best_relative_S.Beta, Vehicles[n].veh_parameter_data_best_relative_S.Tcorr, Vehicles[n].veh_parameter_data_best_relative_S.RT, Vehicles[n].veh_parameter_data_best_relative_S.RelativeError_Velocity);
    //    fprintf(out1_w, "%f    %f    %f    %f    %f    %f    %f    %d    %f\n\n", Vehicles[n].veh_parameter_data_best_relative_S.Gamma, Vehicles[n].veh_parameter_data_best_relative_S.Wm, Vehicles[n].veh_parameter_data_best_relative_S.Wc, Vehicles[n].veh_parameter_data_best_relative_S.Tmax, Vehicles[n].veh_parameter_data_best_relative_S.Alpha, Vehicles[n].veh_parameter_data_best_relative_S.Beta, Vehicles[n].veh_parameter_data_best_relative_S.Tcorr, Vehicles[n].veh_parameter_data_best_relative_S.RT, Vehicles[n].veh_parameter_data_best_relative_S.RelativeError_Gap);
    //    fprintf(out1_w, "%f    %f    %f    %f    %f    %f    %f    %d    %f\n\n", Vehicles[n].veh_parameter_data_best_relative_S.Gamma, Vehicles[n].veh_parameter_data_best_relative_S.Wm, Vehicles[n].veh_parameter_data_best_relative_S.Wc, Vehicles[n].veh_parameter_data_best_relative_S.Tmax, Vehicles[n].veh_parameter_data_best_relative_S.Alpha, Vehicles[n].veh_parameter_data_best_relative_S.Beta, Vehicles[n].veh_parameter_data_best_relative_S.Tcorr, Vehicles[n].veh_parameter_data_best_relative_S.RT, Vehicles[n].veh_parameter_data_best_relative_S.RelativeError_RelativeVelocity);
    //
    //}
    
    ////Output GA - Space Optimization Measure: at Each Time Step for Velocity, Gap and Relative Velocity - Absolute Error
    //for (int n = 0; n < 1 ; n++)//NumOfVehicles is over-estimated by 2
    //{
    //    fprintf(out1_w, "%f    %f    %f    %f    %f    %f    %f    %d    %f\n\n", Vehicles[n].veh_parameter_data_best_absolute_S.Gamma, Vehicles[n].veh_parameter_data_best_relative_S.Wm, Vehicles[n].veh_parameter_data_best_absolute_S.Wc, Vehicles[n].veh_parameter_data_best_absolute_S.Tmax, Vehicles[n].veh_parameter_data_best_absolute_S.Alpha, Vehicles[n].veh_parameter_data_best_absolute_S.Beta, Vehicles[n].veh_parameter_data_best_absolute_S.Tcorr, Vehicles[n].veh_parameter_data_best_absolute_S.RT, Vehicles[n].veh_parameter_data_best_absolute_S.AbsoluteError_Velocity);
    //    fprintf(out1_w, "%f    %f    %f    %f    %f    %f    %f    %d    %f\n\n", Vehicles[n].veh_parameter_data_best_absolute_S.Gamma, Vehicles[n].veh_parameter_data_best_absolute_S.Wm, Vehicles[n].veh_parameter_data_best_absolute_S.Wc, Vehicles[n].veh_parameter_data_best_absolute_S.Tmax, Vehicles[n].veh_parameter_data_best_absolute_S.Alpha, Vehicles[n].veh_parameter_data_best_absolute_S.Beta, Vehicles[n].veh_parameter_data_best_absolute_S.Tcorr, Vehicles[n].veh_parameter_data_best_absolute_S.RT, Vehicles[n].veh_parameter_data_best_absolute_S.AbsoluteError_Gap);
    //    fprintf(out1_w, "%f    %f    %f    %f    %f    %f    %f    %d    %f\n\n", Vehicles[n].veh_parameter_data_best_absolute_S.Gamma, Vehicles[n].veh_parameter_data_best_absolute_S.Wm, Vehicles[n].veh_parameter_data_best_absolute_S.Wc, Vehicles[n].veh_parameter_data_best_absolute_S.Tmax, Vehicles[n].veh_parameter_data_best_absolute_S.Alpha, Vehicles[n].veh_parameter_data_best_absolute_S.Beta, Vehicles[n].veh_parameter_data_best_absolute_S.Tcorr, Vehicles[n].veh_parameter_data_best_absolute_S.RT, Vehicles[n].veh_parameter_data_best_absolute_S.AbsoluteError_RelativeVelocity);
    //
    //}
    
    
    ////Output Different Errors using Space Gap as MOE
    //for (int n = 0; n < 1 ; n++)//NumOfVehicles is over-estimated by 2
    //{
    //    fprintf(out2_w, "%f    %n", Vehicles[n].veh_parameter_data_best_absolute_S.AbsoluteError_Gap);
    //    fprintf(out2_w, "%f    %f    %f    %f    %f    %f    %f    %d    %f\n\n", Vehicles[n].veh_parameter_data_best_absolute_S.Gamma, Vehicles[n].veh_parameter_data_best_absolute_S.Wm, Vehicles[n].veh_parameter_data_best_absolute_S.Wc, Vehicles[n].veh_parameter_data_best_absolute_S.Tmax, Vehicles[n].veh_parameter_data_best_absolute_S.Alpha, Vehicles[n].veh_parameter_data_best_absolute_S.Beta, Vehicles[n].veh_parameter_data_best_absolute_S.Tcorr, Vehicles[n].veh_parameter_data_best_absolute_S.RT);
    //
    //    fprintf(out2_w, "%f    %n", Vehicles[n].veh_parameter_data_best_relative_S.RelativeError_Gap);
    //    fprintf(out2_w, "%f    %f    %f    %f    %f    %f    %f    %d    %f\n\n", Vehicles[n].veh_parameter_data_best_relative_S.Gamma, Vehicles[n].veh_parameter_data_best_relative_S.Wm, Vehicles[n].veh_parameter_data_best_relative_S.Wc, Vehicles[n].veh_parameter_data_best_relative_S.Tmax, Vehicles[n].veh_parameter_data_best_relative_S.Alpha, Vehicles[n].veh_parameter_data_best_relative_S.Beta, Vehicles[n].veh_parameter_data_best_relative_S.Tcorr, Vehicles[n].veh_parameter_data_best_relative_S.RT);
    //
    //    fprintf(out2_w, "%f    %n", Vehicles[n].veh_parameter_data_best_mixed_S.MixedError_Gap);
    //    fprintf(out2_w, "%f    %f    %f    %f    %f    %f    %f    %d    %f\n\n", Vehicles[n].veh_parameter_data_best_mixed_S.Gamma, Vehicles[n].veh_parameter_data_best_mixed_S.Wm, Vehicles[n].veh_parameter_data_best_mixed_S.Wc, Vehicles[n].veh_parameter_data_best_mixed_S.Tmax, Vehicles[n].veh_parameter_data_best_mixed_S.Alpha, Vehicles[n].veh_parameter_data_best_mixed_S.Beta, Vehicles[n].veh_parameter_data_best_mixed_S.Tcorr, Vehicles[n].veh_parameter_data_best_mixed_S.RT);
    //}
    
    
    
    ////Output Sensitivity Analysis Measures
    //for (int n = 0; n < 1 ; n++)//NumOfVehicles is over-estimated by 2
    //{
    //
    //    fprintf(out3_w, "%f    %f    %f    %f    %f    %f    %f    %d    %f\n\n", Vehicles[n].VehParameterSensitivity_Gamma_Mixed[0].Gamma, Vehicles[n].VehParameterSensitivity_Gamma_Mixed[0].Wm, Vehicles[n].VehParameterSensitivity_Gamma_Mixed[0].Wc, Vehicles[n].VehParameterSensitivity_Gamma_Mixed[0].Tmax, Vehicles[n].VehParameterSensitivity_Gamma_Mixed[0].Alpha, Vehicles[n].VehParameterSensitivity_Gamma_Mixed[0].Beta, Vehicles[n].VehParameterSensitivity_Gamma_Mixed[0].Tcorr, Vehicles[n].VehParameterSensitivity_Gamma_Mixed[0].RT);
    //    //for (int i = 0; i < sensitivity[1][1]; i++)
    //    for (int i = 0; i < 30; i++)
    //    {
    //        fprintf(out3_w, "%f    %f    %f    %f    %f    %f    %f    %d    %f    %f\n\n", Vehicles[n].VehParameterSensitivity_Gamma_Mixed[i].Gamma, Vehicles[n].VehParameterSensitivity_Gamma_Mixed[i].Wm, Vehicles[n].VehParameterSensitivity_Gamma_Mixed[i].Wc, Vehicles[n].VehParameterSensitivity_Gamma_Mixed[i].Tmax, Vehicles[n].VehParameterSensitivity_Gamma_Mixed[i].Alpha, Vehicles[n].VehParameterSensitivity_Gamma_Mixed[i].Beta, Vehicles[n].VehParameterSensitivity_Gamma_Mixed[i].Tcorr, Vehicles[n].VehParameterSensitivity_Gamma_Mixed[i].RT,  Vehicles[n].VehParameterSensitivity_Gamma_Mixed[i].MixedError_Gap);
    //    }
    
    //}
    
    
    //*********************** End of Create Output Files ******************************************************************************************
    
    
    fclose(st);
    fclose(out1_w);
    fclose(out2_w);
    fclose(out3_w);
    fclose(out4_w);
    
    //Added by Samer Hamdar: 14 October 2008
    fclose(out);
    fclose(out_R);
    
    fclose(out1);
    fclose(out2);
    fclose(out3);
    fclose(out4);
    fclose(out5);
    fclose(out6);
    
    fclose(out7_1);//Ramp
    fclose(out7_2);
    fclose(out7_3);
    fclose(out7_4);
    fclose(out7_5);
    fclose(out7_6);
    
    fclose(out8_1);//Right
    fclose(out8_2);
    fclose(out8_3);
    fclose(out8_4);
    fclose(out8_5);
    fclose(out8_6);
    
    fclose(out8_1R);//Right-Ramp Proximity
    fclose(out8_2R);
    fclose(out8_3R);
    fclose(out8_4R);
    fclose(out8_5R);
    fclose(out8_6R);
    
    fclose(out9_1);//Left
    fclose(out9_2);
    fclose(out9_3);
    fclose(out9_4);
    fclose(out9_5);
    fclose(out9_6);
    
    fclose(out10_1);//Middle
    fclose(out10_2);
    fclose(out10_3);
    fclose(out10_4);
    fclose(out10_5);
    fclose(out10_6);
    //End of Added by Samer Hamdar: 14 October 2008
    
    return 0;
}

