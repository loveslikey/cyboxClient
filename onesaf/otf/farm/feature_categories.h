/*
 * NOTICE: This file is critical to the OneSAF virtual training use case.  
 * Any changes to this file will be subject to an increased review process
 * to ensure that changes do not impact OneSAF virtual training.  Please 
 * coordinate proposed changes with PM OneSAF to avoid delays in integration.
 */
#ifndef FEATURE_CATEGORIES_H
#define FEATURE_CATEGORIES_H

/*****************************************************************************
**
** File Name:       feature_categories.h
**
** Classification:  Unclassified
**
** Project Name:    OneSAF Software Production
**
** Contract Number: #W900KK-09-D-0323
**
** Task Order:      0007
**
** This work was generated under U.S. Government contract W900KK-09-D-0323,
** and the Government has unlimited data rights therein.
**
** Copyrights:      Copyrights 2006-2015
**                  Leidos 
**                  The Aegis Technologies Group
**                  Cornerstone Software Solutions
**                  Cuspy Engineering Solutions
**                  Dignitas Technologies LLC
**                  GameSim Technologies
**                  Productivity Apex
**                  SoarTech Technology
**                  StackFrame LLC
**                  Lockheed Martin Corporation
**                  Northrop Grumman Corporation
**                  All rights reserved.
**
** DISTRIBUTION STATEMENT C:
**
** Distribution is authorized to the United States Government Agencies and
** USG contractors only (Administrative or Operational Use) (02/02) due to
** critical technology. Other requests shall be referred for consideration
** by  the controlling DoD office.  The controlling DoD office is  the U.S.
** Army PEO STRI Product Manager for One Semi-Automated Forces (PM OneSAF).
**
** Organizations:   Leidos
**                  12901 Science Drive, Orlando, FL  32826-3014
**
** Modifications:   N/A
**
*****************************************************************************/

#include "farm.h"

namespace FARM
{
    class FeatureCategories
    {
      public:

        // Determine and store the feature categories. The FARM class must be
        // intialized before this is called.
        //
        // Return:  Were the feature categories initialized successfully?
        //
        static bool initialize();

        // --------------------------------------------------------------------
        // The remaining methods return the feature category (handle) for the
        // indicated features.
        // --------------------------------------------------------------------

        // Return:  category for areal 'BUILDING'.
        //
        static FeatureCategory areal_building();

        // Return:  category for point 'BUILDING'.
        //
        static FeatureCategory point_building();

        // Return:  category for 'BRIDGE'.
        //
        static FeatureCategory bridge();

        // Return:  category for 'ENGINEER_BRIDGE'.
        //
        static FeatureCategory engineer_bridge();

        // Return:  category for 'BRIDGE_SPAN'.
        //
        static FeatureCategory bridge_span();

        // Return:  category for 'BRIDGE_PIER'.
        //
        static FeatureCategory bridge_pier();

        // Return:  category for areal 'BUILT_UP_REGION'.
        //
        static FeatureCategory areal_built_up_region();

        // Return:  category for 'CAUSEWAY'.
        //
        static FeatureCategory causeway();

        // Return:  category for 'OVERPASS'.
        //
        static FeatureCategory overpass();

        // Return:  category for 'RAILWAY'.
        //
        static FeatureCategory railroad();

        // Return:  category for 'RAILWAY_SIDETRACK'.
        //
        static FeatureCategory railroad_sidetrack();

        // Return:  category for 'ROAD'.
        //
        static FeatureCategory road();

        // Return:  category for 'CART_TRACK'.
        //
        static FeatureCategory cart_track();

        // Return:  category for 'TRAIL'.
        //
        static FeatureCategory trail();

        // Return:  category for 'TERRAIN_CRATER'.
        //
        static FeatureCategory terrain_crater();

        // Return:  category for 'TUNNEL'.
        //
        static FeatureCategory tunnel();

        // Return:  category for linear 'RIVER'.
        //
        static FeatureCategory river();

        // Return:  category for areal 'RIVER'.
        //
        static FeatureCategory areal_river();

        // Return:  category for 'LAKE'.
        //
        static FeatureCategory lake();

        // Return:  category for 'WADI'.
        //
        static FeatureCategory wadi();

        // Return:  category for 'UNDERGROUND_RAILWAY'.
        //
        static FeatureCategory underground_railroad();

        // Return:  category for 'TREED_TRACT'.
        //
        static FeatureCategory treed_tract();

        // Return:  category for 'TREE'.
        //
        static FeatureCategory tree();

        // Return:  category for 'WEAPON_FIGHTING_POSITION'.
        //
        static FeatureCategory weapon_fighting_position();

        // Return:  category for 'INDIVIDUAL_FIGHTING_POSITION'.
        //
        static FeatureCategory individual_fighting_position();

        // Return:  category for point 'SHRUB'.
        //
        static FeatureCategory shrub();

        // Return:  category for 'VEHICLE_LOT'.
        //
        static FeatureCategory vehicle_lot();

        // Return:  category for 'HAZARD_MARKER'.
        //
        static FeatureCategory hazard_marker();

        // Return:  category for 'MINEFIELD'.
        //
        static FeatureCategory minefield();

        // Return:  category for 'MINEFIELD_MARKER'.
        //
        static FeatureCategory minefield_marker();

        // Return:  category for 'point:DRAGON_TEETH'.
        //
        static FeatureCategory dragon_teeth();

        // Return:  category for 'linear:DRAGON_TEETH'.
        //
        static FeatureCategory linear_dragon_teeth();

        // Return:  category for 'RUBBLE'.
        //
        static FeatureCategory rubble();

        // Return:  category for 'LOG_OBSTACLE'.
        //
        static FeatureCategory log_obstacle();

        // Return:  category for 'TERRAIN_OBSTACLE'.
        //
        static FeatureCategory terrain_obstacle();

        // Return:  category for 'ROCK_DROP'.
        //
        static FeatureCategory rock_drop();

        // Return:  category for 'WIRE_OBSTACLE'.
        //
        static FeatureCategory wire_obstacle();

        // Return:  category for 'CROSS_COUNTRY_BARRIER'.
        //
        static FeatureCategory cross_country_barrier();

        // Return:  category for 'ENGINEER_TRENCH'.
        //
        static FeatureCategory engineer_trench();

        // Return:  category for 'INFANTRY_TRENCH'.
        //
        static FeatureCategory infantry_trench();

        // Return:  category for linear 'WALL'.
        //
        static FeatureCategory wall();

        // Return:  category for 'point::BREACH'.
        //
        static FeatureCategory point_breach();

        // Return:  category for 'linear::BREACH'.
        //
        static FeatureCategory linear_breach();

        // Return:  category for 'point::WATER_TOWER'.
        //
        static FeatureCategory water_tower();

        // Return:  category for 'point::COMMUNICATION_TOWER'.
        //
        static FeatureCategory communication_tower();

        // Return:  category for 'point::TENT'.
        //
        static FeatureCategory tent();

        // Return:  category for 'linear::WIRE'.
        //
        static FeatureCategory wire();

        // Return:  category for 'point::DISTURBED_SOIL'.
        //
        static FeatureCategory disturbed_soil();

        // Return:  category for 'linear::SPEED_HUMP'.
        //
        static FeatureCategory speed_hump();

        // Return:  category for 'point::PUMP'.
        //
        static FeatureCategory pump();

        // Return:  category for 'point::STREET_LAMP'.
        //
        static FeatureCategory street_lamp();

        // Return:  category for 'point::DISPLAY_SIGN'.
        //
        static FeatureCategory display_sign();

        // Return:  category for 'areal::GROUND_SURFACE_ELEMENT'.
        //
        static FeatureCategory ground_surface_element();

        // Return:  category for 'areal::TUNNEL_SHELTER'.
        //
        static FeatureCategory tunnel_shelter();

        // Return:  category for 'areal::LAND_FLOODING_PERIODICALLY'.
        //
        static FeatureCategory land_flooding_periodically();

      private:

        static bool
            initialized;

        // If the category elements have not been initialized report an error.
        // Used to factor out the test and error report from the various
        // pseudo-feature methods.
        //
        static void validate();

        static FeatureCategory
            my_areal_building,
            my_point_building,
            my_bridge,
            my_engineer_bridge,
            my_bridge_span,
            my_bridge_pier,
            my_areal_built_up_region,
            my_causeway,
            my_overpass,
            my_railroad,
            my_railroad_sidetrack,
            my_road,
            my_cart_track,
            my_trail,
            my_terrain_crater,
            my_tunnel,
            my_river,
            my_areal_river,
            my_lake,
            my_wadi,
            my_underground_railroad,
            my_treed_tract,
            my_tree,
            my_weapon_fighting_position,
            my_individual_fighting_position,
            my_shrub,
            my_vehicle_lot,
            my_hazard_marker,
            my_minefield,
            my_minefield_marker,
            my_dragon_teeth,
            my_linear_dragon_teeth,
            my_rubble,
            my_log_obstacle,
            my_terrain_obstacle,
            my_rock_drop,
            my_wire_obstacle,
            my_cross_country_barrier,
            my_engineer_trench,
            my_infantry_trench,
            my_wall,
            my_point_breach,
            my_linear_breach,
            my_water_tower,
            my_communication_tower,
            my_tent,
            my_wire,
            my_disturbed_soil,
            my_speed_hump,
            my_pump,
            my_street_lamp,
            my_display_sign,
            my_ground_surface_element,
            my_tunnel_shelter,
            my_land_flooding_periodically;
    };

    // ------------------------------------------------------------------------
    inline void FeatureCategories::validate()
    {
        if (not initialized)
        {
            LOG(fatal, "FeatureCategories used before initialization.");
        }
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::areal_building()
    {
        validate();

        return my_areal_building;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::point_building()
    {
        validate();

        return my_point_building;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::bridge()
    {
        validate();

        return my_bridge;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::engineer_bridge()
    {
        validate();

        return my_engineer_bridge;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::bridge_span()
    {
        validate();

        return my_bridge_span;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::bridge_pier()
    {
        validate();

        return my_bridge_pier;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::areal_built_up_region()
    {
        validate();

        return my_areal_built_up_region;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::causeway()
    {
        validate();

        return my_causeway;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::overpass()
    {
        validate();

        return my_overpass;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::railroad()
    {
        validate();

        return my_railroad;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::railroad_sidetrack()
    {
        validate();

        return my_railroad_sidetrack;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::road()
    {
        validate();

        return my_road;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::cart_track()
    {
        validate();

        return my_cart_track;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::trail()
    {
        validate();

        return my_trail;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::terrain_crater()
    {
        validate();

        return my_terrain_crater;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::tunnel()
    {
        validate();

        return my_tunnel;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::river()
    {
        validate();

        return my_river;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::areal_river()
    {
        validate();

        return my_areal_river;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::lake()
    {
        validate();

        return my_lake;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::wadi()
    {
        validate();

        return my_wadi;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::underground_railroad()
    {
        validate();

        return my_underground_railroad;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::treed_tract()
    {
        validate();

        return my_treed_tract;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::tree()
    {
        validate();

        return my_tree;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::weapon_fighting_position()
    {
        validate();

        return my_weapon_fighting_position;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::individual_fighting_position()
    {
        validate();

        return my_individual_fighting_position;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::shrub()
    {
        validate();

        return my_shrub;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::vehicle_lot()
    {
        validate();

        return my_vehicle_lot;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::hazard_marker()
    {
        validate();

        return my_hazard_marker;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::minefield()
    {
        validate();

        return my_minefield;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::minefield_marker()
    {
        validate();

        return my_minefield_marker;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::dragon_teeth()
    {
        validate();

        return my_dragon_teeth;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::linear_dragon_teeth()
    {
        validate();

        return my_linear_dragon_teeth;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::rubble()
    {
        validate();

        return my_rubble;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::log_obstacle()
    {
        validate();

        return my_log_obstacle;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::terrain_obstacle()
    {
        validate();

        return my_terrain_obstacle;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::rock_drop()
    {
        validate();

        return my_rock_drop;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::wire_obstacle()
    {
        validate();

        return my_wire_obstacle;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::cross_country_barrier()
    {
        validate();

        return my_cross_country_barrier;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::engineer_trench()
    {
        validate();

        return my_engineer_trench;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::infantry_trench()
    {
        validate();

        return my_infantry_trench;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::wall()
    {
        validate();

        return my_wall;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::point_breach()
    {
        validate();

        return my_point_breach;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::linear_breach()
    {
        validate();

        return my_linear_breach;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::water_tower()
    {
        validate();

        return my_water_tower;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::communication_tower()
    {
        validate();

        return my_communication_tower;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::tent()
    {
        validate();

        return my_tent;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::wire()
    {
        validate();

        return my_wire;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::disturbed_soil()
    {
        validate();

        return my_disturbed_soil;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::speed_hump()
    {
        validate();

        return my_speed_hump;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::pump()
    {
        validate();

        return my_pump;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::street_lamp()
    {
        validate();

        return my_street_lamp;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::display_sign()
    {
        validate();

        return my_display_sign;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::ground_surface_element()
    {
        validate();

        return my_ground_surface_element;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::tunnel_shelter()
    {
        validate();

        return my_tunnel_shelter;
    }

    // ------------------------------------------------------------------------
    inline FeatureCategory FeatureCategories::land_flooding_periodically()
    {
        validate();

        return my_land_flooding_periodically;
    }

}

#endif

