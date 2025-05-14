/*
 * NOTICE: This file is critical to the OneSAF virtual training use case.  
 * Any changes to this file will be subject to an increased review process
 * to ensure that changes do not impact OneSAF virtual training.  Please 
 * coordinate proposed changes with PM OneSAF to avoid delays in integration.
 */
/*****************************************************************************
**
** File Name:       feature_categories.cpp
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

#include "feature_categories.h"

namespace
{
    // ------------------------------------------------------------------------
    // Return:  The feature category for the feature label.
    //
    FARM::FeatureCategory get_feature_category(
        const FARM::FeatureLabel &feature_label,
        const FARM::FeatureGeometry geometry
    )
    {
        FARM::Feature
            feature;
        ASSERT(
            FARM::FeatureAttributeMapping::get_feature(
                feature_label, geometry, feature),
            fatal,
            "Could not get the feature for "
            + CORE::to_string(geometry)
            +":" + feature_label
        );

        return feature.get_category();
    }
}

namespace FARM
{
    bool
        FeatureCategories::initialized = false;

    FeatureCategory
        FeatureCategories::my_areal_building,
        FeatureCategories::my_point_building,
        FeatureCategories::my_bridge,
        FeatureCategories::my_engineer_bridge,
        FeatureCategories::my_bridge_span,
        FeatureCategories::my_bridge_pier,
        FeatureCategories::my_areal_built_up_region,
        FeatureCategories::my_causeway,
        FeatureCategories::my_overpass,
        FeatureCategories::my_railroad,
        FeatureCategories::my_railroad_sidetrack,
        FeatureCategories::my_road,
        FeatureCategories::my_cart_track,
        FeatureCategories::my_trail,
        FeatureCategories::my_terrain_crater,
        FeatureCategories::my_tunnel,
        FeatureCategories::my_river,
        FeatureCategories::my_areal_river,
        FeatureCategories::my_lake,
        FeatureCategories::my_wadi,
        FeatureCategories::my_underground_railroad,
        FeatureCategories::my_treed_tract,
        FeatureCategories::my_tree,
        FeatureCategories::my_weapon_fighting_position,
        FeatureCategories::my_individual_fighting_position,
        FeatureCategories::my_shrub,
        FeatureCategories::my_vehicle_lot,
        FeatureCategories::my_hazard_marker,
        FeatureCategories::my_minefield,
        FeatureCategories::my_minefield_marker,
        FeatureCategories::my_dragon_teeth,
        FeatureCategories::my_linear_dragon_teeth,
        FeatureCategories::my_rubble,
        FeatureCategories::my_log_obstacle,
        FeatureCategories::my_terrain_obstacle,
        FeatureCategories::my_rock_drop,
        FeatureCategories::my_wire_obstacle,
        FeatureCategories::my_cross_country_barrier,
        FeatureCategories::my_engineer_trench,
        FeatureCategories::my_infantry_trench,
        FeatureCategories::my_wall,
        FeatureCategories::my_point_breach,
        FeatureCategories::my_linear_breach,
        FeatureCategories::my_water_tower,
        FeatureCategories::my_communication_tower,
        FeatureCategories::my_tent,
        FeatureCategories::my_wire,
        FeatureCategories::my_disturbed_soil,
        FeatureCategories::my_speed_hump,
        FeatureCategories::my_pump,
        FeatureCategories::my_street_lamp,
        FeatureCategories::my_display_sign,
        FeatureCategories::my_ground_surface_element,
        FeatureCategories::my_tunnel_shelter,
        FeatureCategories::my_land_flooding_periodically;

    // ------------------------------------------------------------------------
    bool FeatureCategories::initialize()
    {
        if (FeatureAttributeMapping::initialized())
        {
            initialized = true;

                my_areal_building           = get_feature_category("BUILDING", areal);
            my_point_building               = get_feature_category("BUILDING", point);
            my_bridge                       = get_feature_category("BRIDGE", linear);
            my_engineer_bridge              = get_feature_category("ENGINEER_BRIDGE", linear);
            my_bridge_span                  = get_feature_category("BRIDGE_SPAN", point);
            my_bridge_pier                  = get_feature_category("BRIDGE_PIER", point);
            my_areal_built_up_region        = get_feature_category("BUILT_UP_REGION", areal);
            my_causeway                     = get_feature_category("CAUSEWAY", linear);
            my_overpass                     = get_feature_category("OVERPASS", linear);
            my_railroad                     = get_feature_category("RAILWAY", linear);
            my_railroad_sidetrack           = get_feature_category("RAILWAY_SIDETRACK", linear);
            my_road                         = get_feature_category("ROAD", linear);
            my_cart_track                   = get_feature_category("CART_TRACK", linear);
            my_trail                        = get_feature_category("TRAIL", linear);
            my_terrain_crater               = get_feature_category("TERRAIN_CRATER", point);
            my_tunnel                       = get_feature_category("TUNNEL", linear);
            my_river                        = get_feature_category("RIVER", linear);
            my_areal_river                  = get_feature_category("RIVER", areal);
            my_wadi                         = get_feature_category("WADI", linear);
            my_underground_railroad         = get_feature_category("UNDERGROUND_RAILWAY", linear);
            my_treed_tract                  = get_feature_category("TREED_TRACT", areal);
            my_tree                         = get_feature_category("TREE", point);
            my_weapon_fighting_position     = get_feature_category("WEAPON_FIGHTING_POSITION", point);
            my_individual_fighting_position = get_feature_category("INDIVIDUAL_FIGHTING_POSITION", point);
            my_shrub                        = get_feature_category("SHRUB", point);
            my_vehicle_lot                  = get_feature_category("VEHICLE_LOT", areal);
            my_hazard_marker                = get_feature_category("HAZARD_MARKER", point);
            my_minefield                    = get_feature_category("MINEFIELD", areal);
            my_minefield_marker             = get_feature_category("HAZARD_MARKER", linear);
            my_dragon_teeth                 = get_feature_category("DRAGON_TEETH", point);
            my_linear_dragon_teeth          = get_feature_category("DRAGON_TEETH", linear);
            my_rubble                       = get_feature_category("RUBBLE", point);
            my_log_obstacle                 = get_feature_category("LOG_OBSTACLE", point);
            my_terrain_obstacle             = get_feature_category("TERRAIN_OBSTACLE", point);
            my_rock_drop                    = get_feature_category("ROCK_DROP", point);
            my_wire_obstacle                = get_feature_category("WIRE_OBSTACLE", linear);
            my_cross_country_barrier        = get_feature_category("CROSS_COUNTRY_BARRIER", linear);
            my_engineer_trench              = get_feature_category("ENGINEER_TRENCH", linear);
            my_infantry_trench              = get_feature_category("INFANTRY_TRENCH", linear);
            my_wall                         = get_feature_category("WALL", linear);
            my_point_breach                 = get_feature_category("BREACH", point);
            my_linear_breach                = get_feature_category("BREACH", linear);
            my_water_tower                  = get_feature_category("WATER_TOWER", point);
            my_communication_tower          = get_feature_category("COMMUNICATION_TOWER", point);
            my_wire                         = get_feature_category("WIRE", linear);
            my_disturbed_soil               = get_feature_category("DISTURBED_SOIL", point);
            my_speed_hump                   = get_feature_category("SPEED_HUMP", linear);
            my_pump                         = get_feature_category("PUMP", point);
            my_street_lamp                  = get_feature_category("STREET_LAMP", point);
            my_tent                         = get_feature_category("TENT", point);
            my_display_sign                 = get_feature_category("DISPLAY_SIGN", point);
            my_ground_surface_element       = get_feature_category("GROUND_SURFACE_ELEMENT", areal);
            my_tunnel_shelter               = get_feature_category("TUNNEL_SHELTER", areal);
            my_land_flooding_periodically   = get_feature_category("LAND_FLOODING_PERIODICALLY", areal);

        }

        return FeatureAttributeMapping::initialized();
    }
}

