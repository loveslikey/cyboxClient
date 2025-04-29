/*
 * Classification:  Unclassified
 *
 * Project Name:  OneSAF
 *
 * DISTRIBUTION STATEMENT C.
 *
 * Distribution is authorized to U.S. Government Agencies and their
 * contractors. Export Controlled: 23 August, 2012. Other requests for
 * this document shall be referred to U.S. Army PEO STRI.
 *
 */
/*
 * Copyrights 2006-2018
 *               Leidos
 *               Cole Engineering Services
 *               Cornerstone Software Solutions
 *               Dignitas Technologies LLC
 *               Productivity Apex
 *               SimIS
 *               All rights reserved.
 */
package net.onesaf.core.services.sim.interopmgt.hla.hla13.fom.matrex.converter.common;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;

import javax.vecmath.Vector3d;

import net.onesaf.core.services.data.dm.mdm.mgmt.SimulationStateEnum;
import net.onesaf.core.services.data.dm.objects.ecs.coordinate.GCC;
import net.onesaf.core.services.data.dm.objects.model.FuzeTypeEnum;
import net.onesaf.core.services.data.dm.rdm.env.ERCFeatureChange;
import net.onesaf.core.services.data.dm.rdm.env.FeatureTransient;
import net.onesaf.core.services.data.dm.rdm.org.ActorOwnershipEnum;
import net.onesaf.core.services.data.dm.rdm.phys.LinkStatusEnum;
import net.onesaf.core.services.model.representation.ModelsLibrary;
import net.onesaf.core.services.odm.compsys.CommDevice;
import net.onesaf.core.services.odm.unit.AbstractActor;
import net.onesaf.core.services.odm.unit.AbstractBattlefieldParticipant;
import net.onesaf.core.services.odm.unit.Unit;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.amb.RTIAmbassadorException;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.fedmgt.FederateStatus.FederateStatusEnum;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.message.rti.outgoing.AbstractOutgoingRTIMessage;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.rtihandle.AttributeRTIHandle;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.rtihandle.ObjectClassRTIHandle;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.rtihandle.ObjectRTIHandle;
import net.onesaf.core.services.sim.interopmgt.hla.common.handler.standard.AbstractObjectHandler;
import net.onesaf.core.services.sim.interopmgt.hla.common.handler.standard.converter.FOMObjectConverter;
import net.onesaf.core.services.sim.interopmgt.hla.common.handler.standard.converter.OOSObjectConverter;
import net.onesaf.core.services.sim.interopmgt.hla.common.stream.xdr.XDROutputStream;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.HLA13Interface;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.fom.matrex.converter.util.ActorOwnershipCoordinator;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.fom.matrex.converter.util.PubSubMatrix;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.outgoing.DeleteObjectInstance;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.outgoing.UpdateAttributeValues;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.handler.standard.ObjectHandler;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.service.HLA13HeartbeatService;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.service.HLA13TimeoutService;
import net.onesaf.core.services.sys.alerts.AlertLevel;
import net.onesaf.core.services.sys.alerts.AlertManager;
import net.onesaf.core.services.sys.alerts.AlertParameters;
import net.onesaf.core.services.sys.bso.MunitionTypeEnum;
import net.onesaf.core.services.sys.bso.WarheadTypeEnum;
import net.onesaf.core.services.sys.composition.ComponentManager;
import net.onesaf.core.services.sys.erctypes.EnvironmentHandle;
import net.onesaf.core.services.sys.erctypes.FeatureUUID;
import net.onesaf.core.services.sys.ext.Localization;
import net.onesaf.core.services.sys.id.UniqueID;
import net.onesaf.core.services.sys.logger2.OneSAFLogger;
import net.onesaf.core.services.sys.repository.LocalizedEntry;

/**
 * Base for all object Converters. This class extends CommonConverter to give
 * access to the often use primitive converters. This also gives access to the
 * reference objects stored in AbstractConverter. All MATREX objects to OneSAF
 * objects converters should extend from this converter.
 *
 * @author $Author$
 * @version $Revision$
 */
public abstract class AbstractObjectConverter extends CommonConverter implements
    OOSObjectConverter, FOMObjectConverter {

    /** Reference to the OneSAFLogger object. */
    private static OneSAFLogger logger =
        (OneSAFLogger) OneSAFLogger.getLogger(AbstractObjectConverter.class);

    /**
     * Used when referencing composition models details -- or the utility
     * function to strip the names down to canonical form.
     */
    protected static final ModelsLibrary modelsLibrary = 
                (ModelsLibrary) ComponentManager.getInstance(
                    ModelsLibrary.class.getName());

    /**
     * FOM class name associated with this converter.
     */
    protected String FOM_OBJECT_NAME = null;

    /**
     * ODM class name associated with this converter.
     */
    protected String ODM_OBJECT_NAME = null;

    /** Reference to owning ObjectHandler of this converter. */
    protected static ObjectHandler objectHandler;

    /** Reference to the heartbeating service. */
    protected static HLA13HeartbeatService heartbeatSrvc = null;

    /** Reference to the timeout service. */
    protected static HLA13TimeoutService timeoutSrvc = null;

    /** Flag used to prevent subscribing to OOS more than once. */
    private boolean subscribedToOOS = false;

    /** Set of linked abstract battlefield participants. */
    protected static Set<AbstractBattlefieldParticipant> linkedParticipants = null;

    /** The 'spatial' ODM attribute. */
    protected static final String ODM_SPATIAL = "spatial";

    /** The 'spatial' ODM attribute with respect to HeartBeats. */
    protected static final String ODM_HB_SPATIAL_DATA = "hb_spatial_data";

    /** Flag used to publish non stale spatial data. */
    protected static boolean useDRSpatialData = true;

    /** The "FuelStatus" FOM attribute. */
    protected static final String FOM_FUEL_STATUS = "FuelStatus";

    /** The "AmmoState" FOM attribute. */
    protected static final String FOM_AMMO_STATE = "AmmoState";

    /** The 'basicLoad' OOS attribute. */
    protected static final String ODM_BASIC_LOAD = "basicLoad";
    
    /** The 'actorOwnership' OneSAF attribute. */
    protected static final String ODM_ACTOR_OWNERSHIP = "actorOwnership";
    
    /** The 'actorOwnership' OneSAF attribute. */
    protected static final String FOM_ACTOR_OWNERSHIP = "OwnershipState";

    /**
     * Constant used for referring to the expected converterData value from the
     * ObjectHandlerConfig portion of the config file that indicates how object
     * tags should be enabled.
     */
    protected static final String CONFIG_TAG_GENERATION_METHOD =
        "tagGenerationMethod";
    
    /**
     * Constant used for referring to the expected supplemental id type from the
     * ObjectHandlerConfig portion of the config file that indicates what id type
     * should be populated int he supplemental id field.
     */
    protected static final String CONFIG_SUPPLEMENTAL_ID_TYPE =
        "supplementalIDType";
    
    /**
     * Constant used for referring to the validate locations flag from the
     * ObjectHandlerConfig portion of the config file that indicates what whether
     * or not to validate location values.
     */
    protected static final String CONFIG_VALIDATE_LOCATIONS =
        "validateLocations";
    
    /**
     * Constant used for specifiying whether or not to use non standard FOM UnitIDs.
     */
    protected static final String CONFIG_USE_NON_STANDARD_UNITID =
        "useNonStandardUnitID";

    /**
     * Constant used for referring to the expected converterData value from the
     * ObjectHandlerConfig portion of the config file that indicates the
     * heartbeat services rate in seconds.
     */
    protected static final String CONFIG_HEARTBEAT_USE_DR_SPATIAL_DATA =
        "heartbeatUseDRSpatialData";

    /**
     * String constant used to search the config data to get the value for the
     * default ground platform composition.
     */
    protected static final String CONFIG_DEFAULT_GROUND_COMP =
        "defaultGroundPlatformComposition";

    /**
     * String constant used to search the config data to get the value for the
     * default aircraft composition.
     */
    protected static final String CONFIG_DEFAULT_AIRCRAFT_COMP =
        "defaultAircraftComposition";

    /**
     * String constant used to search the config data to get the value for the
     * default IC composition.
     */
    protected static final String CONFIG_DEFAULT_IC_COMP =
        "defaultICComposition";

    /**
     * String constant used to search the config data to get the value for the
     * default munition composition.
     */
    protected static final String CONFIG_DEFAULT_MUNITION_COMP =
        "defaultMunitionComposition";

    /**
     * String constant used to search the config data to get the value for the
     * default unit composition.
     */
    protected static final String CONFIG_DEFAULT_UNIT_COMP =
        "defaultUnitComposition";

    /**
     * String constant used to search the config data to get the value for the
     * default platform type enumeration to use.
     */
    protected static final String CONFIG_PLATFORM_ENUM = "platformEnum";

    /**
     * String constant used to search the config data to get the value for the
     * default aircraft IC type enumeration to use.
     */
    protected static final String CONFIG_IC_ENUM = "icEnum";

    /**
     * String constant used to search the config data to get the value for the
     * default munition type enum to use.
     */
    protected static final String CONFIG_MUNITION_ENUM = "munitionEnum";

    /**
     * String constant used to search the config data to get the value for the
     * default articulated parts publish/subscribe setting to use.
     */
    protected static final String CONFIG_ARTICULATED_PARTS_SETTING =
        "articulatedPartsSetting";

    /**
     * String constant used to determine if object istance name should be used
     * for ID lookups.
     */
    protected static final String CONFIG_USE_OBJECT_INSTANCE_NAME =
        "useObjectInstanceNameForIDLookup";

    /** The articulated parts set on. */
    public static boolean articulatedPartsSetOn = false;

    /** The FOM attribute for ArticulatingParts on a Platform Object. */
    protected static final String FOM_ARTICULATED_PART = "ArticulatingParts";

    /** The ODM attribute for ArticulatedParts on a PhysicalEntity. */
    protected static final String ODM_ARTICULATED_PART = "articulatedPart";
    /**
     * ODM object properties that are being listened to that will require
     * updates to be published over interop. Populated by subclass.
     */
    protected Set<String> subODMAttrs;

    /**
     * ODM properties that are being listened to that will require updates via
     * heartbeats to the FOM.
     */
    protected Set<String> subODMHBAttrs;

    /**
     * The attribtues of the FOM object that will be published by the converter.
     * Populated by subclass.
     */
    protected Set<String> pubFOMAttrs;

    /**
     * The attribtues of the FOM object that will be subsrcibed to by the
     * converter. Populated by subclass.
     */
    protected Set<String> subFOMAttrs;

    /**
     * Objects discovered through interop that have not provided enough
     * information to be created are stored in this map, When the proper
     * attributes are received, the entity will be created and removed from this
     * map.
     **/
    protected Map<ObjectRTIHandle, Parameters> pendingObjects;
    
    private static final LocalizedEntry RECEIVED_AVAILBLE_FOR_SUBORDINATE = 
        new LocalizedEntry("External unit %s is available to take over ownership of local actors as subordinates");
    
    private static final LocalizedEntry AVAILBLE_FOR_SUBORDINATE_INSTRUCTION = 
            new LocalizedEntry("Please task organize local units or entities, that you wish to give up ownership control, under the external unit %s");

    /** Whether this object should be published */
    private boolean shouldPublish = true;

    private final String UNKNOWN = "Unknown";

    /**
     * Sub-classes should add/remove parameters to the pub/sub sets after
     * initializing this class.
     * 
     * @param objHandler AbstractObjectHandler that created this converter
     */
    @Override
    public void initialize(AbstractObjectHandler objHandler) {

        pubFOMAttrs = new HashSet<String>();
        subFOMAttrs = new HashSet<String>();
        subODMAttrs = new HashSet<String>();
        subODMHBAttrs = new HashSet<String>();
        pendingObjects = new HashMap<ObjectRTIHandle, Parameters>();

        if (objectHandler == null) {

            super.initialize(objHandler.getTranslationManager());
            objectHandler = (ObjectHandler) objHandler;
            linkedParticipants = new HashSet<AbstractBattlefieldParticipant>();

            // Get converter data map (String -> String) from config file
            Map<String, String> converterData =
                objectHandler.getConverterData();

            DEFAULT_GROUND_PLATFORM_COMPOSITION =
                converterData.get(CONFIG_DEFAULT_GROUND_COMP);
            if (DEFAULT_GROUND_PLATFORM_COMPOSITION == null
                || DEFAULT_GROUND_PLATFORM_COMPOSITION.equals("")) {
                logger.error("defaultGroundPlatformComposition was not "
                    + "defined in the interop config file");
            }

            DEFAULT_AIRCRAFT_COMPOSITION =
                converterData.get(CONFIG_DEFAULT_AIRCRAFT_COMP);
            if (DEFAULT_AIRCRAFT_COMPOSITION == null
                || DEFAULT_AIRCRAFT_COMPOSITION.equals("")) {
                logger.error("defaultAircraftComposition was not "
                    + "defined in the interop config file");
            }

            DEFAULT_IC_COMPOSITION =
                converterData.get(CONFIG_DEFAULT_IC_COMP);
            if (DEFAULT_IC_COMPOSITION == null
                || DEFAULT_IC_COMPOSITION.equals("")) {
                logger.error("defaultICComposition was not "
                    + "defined in the interop config file");
            }

            DEFAULT_MUNITION_COMPOSITION =
                converterData.get(CONFIG_DEFAULT_MUNITION_COMP);
            if (DEFAULT_MUNITION_COMPOSITION == null
                || DEFAULT_MUNITION_COMPOSITION.equals("")) {
                logger.error("defaultMunitionComposition was not "
                    + "defined in the interop config file");
            }

            DEFAULT_UNIT_COMPOSITION =
                converterData.get(CONFIG_DEFAULT_UNIT_COMP);
            if (DEFAULT_UNIT_COMPOSITION == null
                || DEFAULT_UNIT_COMPOSITION.equals("")) {
                logger.error("defaultUnitComposition was not "
                    + "defined in the interop config file");
            }

            PLATFORM_ENUM = converterData.get(CONFIG_PLATFORM_ENUM);
            if (PLATFORM_ENUM == null || PLATFORM_ENUM.equals("")) {
                logger.error(CONFIG_PLATFORM_ENUM + " was not "
                    + "defined in the interop config file");
            }

            IC_ENUM = converterData.get(CONFIG_IC_ENUM);
            if (IC_ENUM == null || IC_ENUM.equals("")) {
                logger.error(CONFIG_IC_ENUM + " was not "
                    + "defined in the interop config file");
            }

            MUNITION_ENUM = converterData.get(CONFIG_MUNITION_ENUM);
            if (MUNITION_ENUM == null || MUNITION_ENUM.equals("")) {
                logger.error(CONFIG_MUNITION_ENUM + " was not "
                    + "defined in the interop config file");
            }

            ARTICULATED_PARTS_SETTING =
                converterData.get(CONFIG_ARTICULATED_PARTS_SETTING);
            if (ARTICULATED_PARTS_SETTING == null
                || ARTICULATED_PARTS_SETTING.equals("")) {
            	if(logger.isDebugEnabled()) {
	                logger.debug(CONFIG_ARTICULATED_PARTS_SETTING + " was not "
	                    + "defined in the interop config file");
            	}
            } else if (ARTICULATED_PARTS_SETTING.matches("ON")) {

                articulatedPartsSetOn = true;
            }

            String stringData =
                converterData.get(CONFIG_TAG_GENERATION_METHOD);
            if (stringData != null) {

                mtxSimulationServices.setTagGenerationMethod(stringData);
            }
            
            stringData =
                    converterData.get(CONFIG_SUPPLEMENTAL_ID_TYPE);
            if (stringData != null) {

                mtxSimulationServices.setSupplementalIDType(stringData);
            }
            
            stringData =
                    converterData.get(CONFIG_VALIDATE_LOCATIONS);
            if (stringData != null) {

                mtxSimulationServices.setValidateLocations(Boolean
                        .parseBoolean(stringData));
            }  
            
            stringData =
                    converterData.get(CONFIG_USE_NON_STANDARD_UNITID);
            if (stringData != null) {

                mtxSimulationServices.setUseNonStandardUnitID(Boolean
                        .parseBoolean(stringData));
            } 
            
            stringData =
                converterData.get(CONFIG_HEARTBEAT_USE_DR_SPATIAL_DATA);
            if (stringData != null) {
                try {
                    useDRSpatialData = Boolean.parseBoolean(stringData);
                } catch (Exception e) {
                    logger.error("Invalid value for "
                        + CONFIG_HEARTBEAT_USE_DR_SPATIAL_DATA
                        + " in interop config file");
                }
            }

            stringData = converterData.get(CONFIG_USE_OBJECT_INSTANCE_NAME);
            if (stringData != null) {
                try {
                    mtxSimulationServices
                        .setObjectInstanceNameForLookups(Boolean
                            .parseBoolean(stringData));
                } catch (Exception e) {
                    logger.error("Invalid value for "
                        + CONFIG_USE_OBJECT_INSTANCE_NAME
                        + " in interop config file");
                }
            }

            // handle initial sim state
            simStateChanged(oosAmbassador.getSimulationState());
        }
    }

    /**
     * Return the full name of the FOM Object class that this converter publish
     * and/or subscribe to. (example ObjectRoot.Platform.GroundVehicle)
     *
     * @return String full name of the FOM object class.
     */
    protected String getFOMObjectName() {

        return FOM_OBJECT_NAME;
    }

    /**
     * Return the full class path of OOS object that will be associated to the
     * FOM class returned by getFOMObjectName. (example
     * net.onesaf.core.services.odm.entity.GroundVehicle)
     *
     * @return String the OOS object to be associated with the FOM object clas.
     */
    protected String getODMObjectName() {

        return ODM_OBJECT_NAME;
    }

    /**
     * Interface method to be called from the ObjectHandler on OBJECT_CREATE
     * messages. Calls back to the Object Handler to record the object into its
     * tables.
     *
     * @param objCreated the obj created
     */
    @Override
    public void processOOSCreate(Object objCreated) {

    	
        // Call back to ObjectHandler
        if (objCreated instanceof AbstractBattlefieldParticipant) {

            AbstractBattlefieldParticipant abp = 
                (AbstractBattlefieldParticipant) objCreated;
            LinkStatusEnum linkStatus = abp.getLinkStatus();
            
            if (objCreated instanceof Unit) {
                mtxSimulationServices.addLocalUnitToMap((Unit)abp);
            } else {
                mtxSimulationServices.addEntityToMap(abp);
            }

            if (LinkStatusEnum.NA == linkStatus) {

                String objTag = nextObjectTag(objCreated);

                if (objTag == null) {

                    objectHandler.getNextObjectTag();
                }

                objectHandler.recordNewObject(abp, getFOMObjectName(),
                    objTag);
            } else if (LinkStatusEnum.LINKED == linkStatus
                && !linkedParticipants.contains(abp)) {

                abp.setLinkStatus(LinkStatusEnum.UNLINKED);
            }
        } else if (objCreated instanceof ERCFeatureChange) {
        	
        	
        	ERCFeatureChange
            	featureChange = (ERCFeatureChange) objCreated;
        	try {

                FeatureUUID
                    fid = featureChange.getUUID();

                EnvironmentHandle
                    eh = featureChange.getEnvironmentHandle();
        	
                if (eh != null && eh.valid()) {
        		
        		
                	String objTag = nextObjectTag(objCreated);

                	if (objTag == null) {

                		objectHandler.getNextObjectTag();
                	}
                
                	objectHandler.recordNewObject(eh, getFOMObjectName(),
                			objTag);
        		
                }
        	} catch (Exception ex) {
            logger.error("Unable to retrieve FeatureName from " +
                         "ERCFeatureChange",ex);
        	}
        } else if (objCreated instanceof FeatureTransient) {
        	FeatureTransient trans = (FeatureTransient) objCreated;
        	
        	try {

                FeatureUUID
                    fid = trans.getUUID();

                EnvironmentHandle
                    eh = new EnvironmentHandle(
                        fid,
                        trans.getPageId(),
                        trans.getFeatureIndex(),
                        trans.getSeqGeotileId());
        	
                if (eh != null && eh.valid()) {
        		
        		
                	String objTag = nextObjectTag(objCreated);

                	if (objTag == null) {

                		objectHandler.getNextObjectTag();
                	}
                
                	objectHandler.recordNewObject(eh, getFOMObjectName(),
                			objTag);
        		
                }
        	} catch (Exception ex) {
        		logger.error("Unable to retrieve FeatureName from " +
                         "ERCFeatureChange",ex);
        	}
        }
        else if (objCreated instanceof CommDevice)
        {
        	String objTag = nextObjectTag(objCreated);
        	objectHandler.recordNewObject(objCreated, getFOMObjectName(), objTag);
        }
    }

    /**
     * Return the next object tag to be used. Subclasses can overload this
     * method for specific naming conventions.
     * 
     * @param objCreated Object the object being published over interop needing
     *        an object tag.
     * @return String The object tag to use.
     */
    public String nextObjectTag(Object objCreated) {

        return objectHandler.getNextObjectTag();
    }

    /**
     * Interface method to be called from the ObjectHandler on OBJECT_REMOVE
     * messages. Calls back to the Object Handler to remove the object from its
     * tables.
     *
     * @param objRemoved the obj removed
     */
    @Override
    public void processOOSRemove(Object objRemoved) {

        ObjectRTIHandle proxyHandle = rtiHandleMngr.getRTIProxy(objRemoved);

        if (objRemoved != null) {

            if (objectHandler.recordRemovedOOSObject(objRemoved)) {

                if (objRemoved instanceof AbstractBattlefieldParticipant) {
                    mtxSimulationServices.removeEntityFromMap((AbstractBattlefieldParticipant)objRemoved);
                }
                // Alert heartbeat service to the object removal
                if (heartbeatSrvc == null) {
                    if (debugEnabled) {
                        logger
                            .debug("No heartbeat services running to be notified "
                                + "of OOS remove.");
                    }
                } else {
                    heartbeatSrvc.removeOOSObject(proxyHandle);
                }
            
                // Create & send "DeleteObjectInstance" message to RTI
                DeleteObjectInstance deleteObjectInstance = new DeleteObjectInstance();

                deleteObjectInstance.setObjectHandle(proxyHandle);
                deleteObjectInstance.setTag(objectHandler.strTagToByteArray(objectHandler
                    .getObjectTag(proxyHandle)));

                deleteObjectInstance.send();
            }
        }
    }

    /**
     * Perform all necessary conversion between an updated ODM property and any
     * related FOM attributes.
     * 
     * @param object Object OOS entity being with changed properties.
     * @param changedProperties Set indicating which properties have
     *        changed.
     */
    @Override
    public void processOOSPropertyChange(Object object,
        Set<String> changedProperties) {

        if (debugEnabled) {
            logger.debug("AbstractObjectConverter.processOOSPropertyChange()");
            logger.debug(" properties changed: " + changedProperties);
            logger.debug("converter instance class = "
                + this.getClass().getName());
        }

        // Get a new RTI handle for the object
        ObjectClassRTIHandle classHandle = null;
        try {
            classHandle =
                rtiHandleMngr.getObjectClassHandle(getFOMObjectName());
        } catch (RTIAmbassadorException rtiEx) {
            logger.error("Error getting FOM object class: "
                + getFOMObjectName(), rtiEx);
            return;
        }

        // Create container for attribute names and the associated data byte
        // arrays to be sent to the RTI.
        Map<AttributeRTIHandle, byte[]> outputData =
            new HashMap<AttributeRTIHandle, byte[]>();

        // Iterate over all property changes & pass off to converter
        for (String changedProperty : changedProperties) {

            handleODMPropertyChanged(classHandle, object, changedProperty,
                outputData);
        }

        if (!outputData.isEmpty()) {

            finishedODMPropertyChanges(classHandle, object, outputData);
            // Write resulting outputData map to the FOM
            writeToFOM(object, outputData);
        }
    }

    /**
     * Interface method to be called when a PROVIDE_ATTRIBUTE_VALUE_UPDATE
     * message is received. Handles the conversion of all the requested FOM
     * attributes to OOS attributes and provides the corresponding
     * UpdateAttributeValues message to the RTI. Also calls template-method
     * hooks for subclasses to convert any additional attributes that this class
     * does not support.
     *
     * @param objectHandle the object handle
     * @param attrs the attrs
     */
    @Override
    public final void processFOMAttrRequest(ObjectRTIHandle objectHandle,
        Set<AttributeRTIHandle> attrs) {

        if (debugEnabled) {
            logger.debug("AbstractObjectConverter.processFOMAttrRequest()");
        }

        if (objectHandle.getProxy() == null) {
            logger.error("FOM attribute request for RTIHandle with no "
                + "proxy - aborting conversion.");
            return;
        }

        Object proxyObject = objectHandle.getProxy();

        // Get a new RTI handle for the object
        ObjectClassRTIHandle classHandle;
        try {
            classHandle =
                rtiHandleMngr.getObjectClassHandle(getFOMObjectName());
        } catch (RTIAmbassadorException rtiEx) {
            logger.error("RTI Error: ", rtiEx);
            return;
        }

        // Create container for attribute names and the associated data byte
        // arrays to be sent to the RTI.
        Map<AttributeRTIHandle, byte[]> outputData =
            new HashMap<AttributeRTIHandle, byte[]>();

        Set<String> changedProperties = new HashSet<String>();
        for (AttributeRTIHandle attr : attrs) {
        	// Note that the RTI Federate Ambassador documentation is silent on what to
        	// do in case of a null attr request.  For now, drop the attr.
        	if (attr != null) {
                changedProperties.add(attr.getName());
        	}
        }

        // Iterate over all property changes & pass off to converter
        for (String changedProperty : changedProperties) {

            handleFOMAttrRequest(classHandle, proxyObject, changedProperty,
                outputData);
        }

        if (!outputData.isEmpty()) {

            finishedODMPropertyChanges(classHandle, proxyObject, outputData);
            // Write resulting outputData map to the FOM
            writeToFOM(proxyObject, outputData);
        }
    }

    /**
     * Method allows a request to update <code>attributeName</code> from
     * <code>proxyObject</code> which is represented by <code>classHandle</code>
     * and written to <code>outputData</code>. Subclasses should override this
     * method to process attributes specific to that converter.
     * <code>attributeName</code> represents a FOM object attribute name.
     * 
     * @param classHandle RTIHandle representing the FOM class represented by
     *        proxyObject
     * @param proxyObject Object the OneSAF object that will provide the
     *        attribute data.
     * @param attributeName String name of the attribute on the FOM class that
     *        is being requested.
     * @param outputData map of attribute handles to encoded data.
     * @param tag the tag from the publishing federate
     */
    protected void handleFOMAttrRequest(ObjectClassRTIHandle classHandle,
        Object proxyObject, String attributeName,
        Map<AttributeRTIHandle, byte[]> outputData, byte[] tag) {

    }

    /**
     * Method allows a request to update <code>attributeName</code> from
     * <code>proxyObject</code> which is represented by <code>classHandle</code>
     * and written to <code>outputData</code>. Subclasses should override this
     * method to process attributes specific to that converter.
     * <code>attributeName</code> represents a FOM object attribute name.
     * 
     * @param classHandle RTIHandle representing the FOM class represented by
     *        proxyObject
     * @param proxyObject Object the OneSAF object that will provide the
     *        attribute data.
     * @param attributeName String name of the attribute on the FOM class that
     *        is being requested.
     * @param outputData map of attribute handles to encoded data.
     */
    protected void handleFOMAttrRequest(ObjectClassRTIHandle classHandle,
        Object proxyObject, String attributeName,
        Map<AttributeRTIHandle, byte[]> outputData) {
        byte[] tag = UNKNOWN.getBytes();
        handleFOMAttrRequest(classHandle, proxyObject, attributeName,
                             outputData, tag);
    }

    /**
     * Method to allow subclasses to do any post processing upon property
     * changes.
     * 
     * @param classHandle RTIHandle associated with FOM class represented by
     *        object.
     * @param object Object that provides the data for the attribtue update.
     * @param outputData map of attribute handles to encoded data.
     */
    protected void finishedODMPropertyChanges(ObjectClassRTIHandle classHandle,
        Object object, Map<AttributeRTIHandle, byte[]> outputData) {

    }

    /**
     * Handle the updated OOS <code>changedProperty</code> on
     * <code>object</code>. If it is an attribute of concern, then it will be
     * XDR encoded and and stored in <code>outputData</code>.
     * 
     * @param classHandle RTIHandle representing the FOM class represented by
     *        proxyObject
     * @param object Object the OneSAF object that will provide the attribute
     *        data.
     * @param changedProperty String name of the property changed on object
     * @param outputData map of attribute handles to encoded data.
     */
    protected void handleODMPropertyChanged(ObjectClassRTIHandle classHandle,
        Object object, String changedProperty,
        Map<AttributeRTIHandle, byte[]> outputData) {

        if (debugEnabled) {

            logger.debug(changedProperty + " for " + getODMObjectName()
                + " is not supported for the FOM Object " + getFOMObjectName());
        }
    }

    protected boolean shouldSendUpdate(Object object, Map<AttributeRTIHandle, byte[]> outputData) {
        return true;
    }

    /**
     * If the discovered object correlates to an existing entity, set it as the
     * proxy on the RTIHandle and set the link status to linked.
     *
     * @param rtiHandle the rti handle
     * @param objectName the object name
     */
    @Override
    public void processFOMDiscover(ObjectRTIHandle rtiHandle, String objectName) {

        // Cannot create OOS object b/c don't know the Type yet which is
        // needed to determine the composition. For new objects, wait for the
        // following update. If late-joining & getting a discover, wait for the
        // next heartbeat.
        logger.info(
            "Discovered an " + getFOMObjectName () + " named " + objectName);

        discoveredFOMObjects.add(rtiHandle);
    }

    /**
     * Process fom remove.
     *
     * @param rtiHandle the rti handle
     */
    @Override
    public void processFOMRemove(ObjectRTIHandle rtiHandle) {

        // Alert timeout service to remove an entry for this object
        if (timeoutSrvc != null) {
            timeoutSrvc.removeRTIObject(rtiHandle);
        }

        Object obj = rtiHandle.getProxy();
        logger.info(
            "Removing an " + getFOMObjectName () + " named " + 
            	    rtiHandle.getName());
        
        ActorOwnershipCoordinator coordinator = ActorOwnershipCoordinator.getInstance();

        if (obj instanceof AbstractActor && coordinator.isExternalActorInTransfer((AbstractActor)obj)) {
            objectHandler.recordRemovedFOMObject(rtiHandle);
            coordinator.makeExternalToLocal((AbstractActor) obj, oosAmbassador);
        } else {
            if (obj != null && obj instanceof AbstractBattlefieldParticipant) {

                AbstractBattlefieldParticipant abp = 
                        (AbstractBattlefieldParticipant) obj;

                if (abp.getLinkStatus() == LinkStatusEnum.LINKED) {

                    abp.setLinkStatus(LinkStatusEnum.UNLINKED);
                    linkedParticipants.remove(abp);
                } else {

                    objectHandler.recordRemovedFOMObject(rtiHandle);
                    objectHandler.destroyGhostODMObject(abp);
                }
            } else {

                objectHandler.recordRemovedFOMObject(rtiHandle);
                objectHandler.destroyGhostODMObject(obj);
            }
        }

        discoveredFOMObjects.remove(rtiHandle);
        rtiHandle.removeProxy();
    }

    /**
     * Internal method to handle processing of object attributes. Should be
     * defined in all sub-classes with attributes to process, that do not define
     * processFOMReflectAttr.
     * 
     * @param proxy OneSAF object to receive the attribute data.
     * @param attributeName attibute to be processed.
     * @param encodedData the XDR encoded bytes for the attribute.
     * @param tag the tag from the publishing federate
     * @see #processFOMReflectAttr
     */
    protected void handleFOMAttr(Object proxy, String attributeName,
        byte[] encodedData, byte[] tag) {

        if (debugEnabled) {

            logger.debug(attributeName
                + " is not supported for the FOM Object " + getFOMObjectName());
        }
    }

    /**
     * Internal method to handle processing of object attributes. Should be
     * defined in all sub-classes with attributes to process, that do not define
     * processFOMReflectAttr.
     * 
     * @param proxy OneSAF object to receive the attribute data.
     * @param attributeName attibute to be processed.
     * @param encodedData the XDR encoded bytes for the attribute.
     * @see #processFOMReflectAttr
     */
    protected void handleFOMAttr(Object proxy, String attributeName,
        byte[] encodedData) {

        byte[] tag = UNKNOWN.getBytes();
        handleFOMAttr(proxy, attributeName, encodedData, tag);
    }

    /**
     * Interface method to be called when a REFLECT_ATTRIBUTE_VALUES message is
     * received. Will iterate through the incoming attributes and call
     * handleFOMAttr. handleFOMAttr should be defined in subclasses. If this
     * method is defined in a subclass, it will be the up to the sub-class on
     * how to handle processing of the attributes. This may be necessary for
     * objects that need to create an ODM object to associate with RTIHandle.
     * 
     * @param objRTIHandle RTIHandle associated with the incoming attribute.
     * @param attributes Map of attributes and the XDR encoded bytes associated
     *        to be processed.
     * @param tag byte[] tag for the reflect attribute message.
     * @see #handleFOMAttr
     */
    @Override
    public void processFOMReflectAttr(ObjectRTIHandle objRTIHandle,
        Map<AttributeRTIHandle, byte[]> attributes, byte[] tag) {

        if (!validFOMObject(objRTIHandle)) {

            // Never received discover notification for this object or this
            // object has been removed. Nothing to do.
            return;
        }

        if (debugEnabled) {
            logger.debug("AbstractObjectHandler.processFOMReflectAttr()");
        }

        // Alert timeout service to the object update
        if (timeoutSrvc != null) {
            timeoutSrvc.receiveRTIUpdate(objRTIHandle);
        }

        Object proxy = objRTIHandle.getProxy();

        // 'attributes' => Map (RTIHandle -> byte[])
        for (Iterator<Map.Entry<AttributeRTIHandle, byte[]>> i =
            attributes.entrySet().iterator(); i.hasNext();) {

            Map.Entry<AttributeRTIHandle, byte[]> item = i.next();
            AttributeRTIHandle attrHandle = item.getKey();
            byte[] encodedData = item.getValue();
            String attributeName = attrHandle.getName();

            if (debugEnabled) {
                logger.debug("    attributeName = " + attributeName);
            }

            handleFOMAttr(proxy, attributeName, encodedData, tag);
        }
    }

    /**
     * Creates an UpdateAttributeValues message to write the XDR encoded data to
     * the FOM. If there is a heart beat service, it will be notified of the
     * update.
     * 
     * @param object Object that is the OneSAF representation of the FOM object
     *        being updated.
     * @param outputData map of attributes to encoded
     *        data to set in the UpdateAttributeValues message.
     */
    protected void writeToFOM(Object object,
        Map<AttributeRTIHandle, byte[]> outputData) {

        if (! shouldPublish) {
            // The pub/sub matrix has selected to not publish this object
            return;
        }

        if (! shouldSendUpdate(object, outputData)) {
            // The converter doesn't want this update to be published
            return;
        }

        ObjectRTIHandle objectHandle = rtiHandleMngr.getRTIProxy(object);

        // Create and send a new "UpdateAttributeValues" message to the RTI.
        UpdateAttributeValues updateAttrMsg = new UpdateAttributeValues();

        updateAttrMsg.setObjectHandle(objectHandle);
        updateAttrMsg.setTag(objectHandler.strTagToByteArray(getFederationData()
            .getFederateName()));
        updateAttrMsg.setAttrValueMap(outputData);

        // Alert heartbeat services to new message
        if (heartbeatSrvc == null) {
            if (debugEnabled) {
                logger.debug("No heartbeat services running to be notified "
                    + "of OOS update.");
            }
        } else {
            heartbeatSrvc.receiveOOSUpdate(updateAttrMsg);
        }

        // Send message to the RTI
        updateAttrMsg.send();
    }

    /**
     * Send an attribute update on all the attributes after the object was
     * registered with the RTI.
     *
     * @param object the object
     * @param rtiHandle the rti handle
     */
    @Override
    public void postRTIRegistrationHook(Object object, ObjectRTIHandle rtiHandle) {

        processOOSPropertyChange(object, subODMAttrs);
    }

    /**
     * Method to trigger when to publish/subscribe according to a simulation
     * state.
     *
     * @param simState the sim state
     */
    @Override
    public void simStateChanged(SimulationStateEnum simState) {

        if (!subscribedToOOS
            && (SimulationStateEnum.LOADED == simState
                || SimulationStateEnum.RUNNING == simState
                || SimulationStateEnum.RESTORED == simState || SimulationStateEnum.INITIALIZED == simState)) {

            subscribedToOOS = true;

            // subscribe to OOS Object
            String subName = getODMObjectName();

            if (subName != null) {
                if (debugEnabled) {
                    logger.debug("Subscribing to OOS Object: " + subName);
                }
                objectHandler.subscribeOOSObject(subName, this);
            }
        }
    }

    /**
     * Method to respond to the initial simulation state.
     *
     * @param simState the sim state
     */
    @Override
    public void initialSimState(SimulationStateEnum simState) {

        simStateChanged(simState);
    }

    /**
     * Upon RTI join, FOM pub/sub, and initialize the heartbeat and timeout
     * services. Upon RTI resign, Stop the heartbeat and timeout services.
     * Subclasses should EXTEND this method if they have anything else to do.
     *
     * @param fedStatus the fed status
     */
    @Override
    public void fedStatusChanged(FederateStatusEnum fedStatus) {

        if ((FederateStatusEnum.POST_JOIN == fedStatus)
            && (getFOMObjectName() != null)) {

            PubSubMatrix pubSubMatrix = PubSubMatrix.getInstance();

            if (pubFOMAttrs.size() > 0) {
                shouldPublish = pubSubMatrix.isHlaObjectPub(getFOMObjectName());
            } else {
                shouldPublish = false;
            }
            boolean shouldSubscribe = false;

            if (subFOMAttrs.size() > 0) {
                shouldSubscribe = pubSubMatrix.isHlaObjectSub(getFOMObjectName());
            }

            if (hlaManager.getHLAInterface() instanceof HLA13Interface) {
                HLA13Interface iface = (HLA13Interface)hlaManager.getHLAInterface();
               // iface.addPubSubInfo(getFOMObjectName(), true, shouldPublish, shouldSubscribe); // true means is-an-object
            }

            // publish/subscribe to FOM Object (require subclasses to
            // override methods used in these calls)
            if (shouldPublish) {
                objectHandler.publishFOMObject(getFOMObjectName(), pubFOMAttrs);
            }

            if (shouldSubscribe) {
                objectHandler.subscribeFOMObject(getFOMObjectName(),
                    subFOMAttrs, this);
            }

            // Get reference to the heartbeat & timeout services
            heartbeatSrvc =
                (HLA13HeartbeatService) objectHandler.getHeartbeatService();
            timeoutSrvc =
                (HLA13TimeoutService) objectHandler.getTimeoutService();
        } else if (FederateStatusEnum.PRE_RESIGN == fedStatus) {

            // NOTE: Method called once for each subclass
            // - set service references to 'null' to prevent multiple
            // stop()-ages
            if (heartbeatSrvc != null) {
                heartbeatSrvc.stop();
                heartbeatSrvc = null;
            }
            if (timeoutSrvc != null) {
                timeoutSrvc.stop();
                timeoutSrvc = null;
            }
        }

        handleFedStatusChange(fedStatus);
    }

    /**
     * Class used to represent the attributes associated with a FOM class. A
     * list of required attributes can be set to provide a minimum set of
     * attributes that are needed to represent the FOM class as a OneSAF object.
     * This clas can store the encoded data associated with an attribute until
     * all the required attributes are available to create the OneSAF
     * representation.
     */
    public class Parameters {

        // Map of attributes to XDR encoded data.
        private Map<String, byte[]> attributes = new HashMap<String, byte[]>();

        // List of required attributes needed to create the OneSAF object.
        private List<String> requiredAttributes = new ArrayList<String>();

        /**
         * Return a map of the attributes and the associated XDR encoded data
         * that were stored in this class.
         *
         * @return received attributes and the associated data.
         */
        public Map<String, byte[]> getAttributes() {

            return attributes;
        }

        /**
         * Store the XDR encoded <code>data</code> for the attribute represented
         * by <code>name</code>.
         * 
         * @param name String the name of the FOM attribute that data is
         *        associated with.
         * @param data byte[] XDR encoded data.
         */
        public void setAttribute(String name, byte[] data) {

            attributes.put(name, data);
        }

        /**
         * Mark <code>attributeName</code> as a required attribute. All
         * attributes marked as required, will have to be set for isValid to
         * return true.
         *
         * @param attributeName String the name of the FOM attribute to mark as
         *        required
         */
        public void setRequiredAttribute(String attributeName) {

            requiredAttributes.add(attributeName);
        }

        /**
         * Removes <code>attributeName</code> from the required set of
         * attributes.
         * 
         * @param attributeName String the name of the FOM attribute to remove
         *        the required set of attributes.
         */
        public void removeRequiredAttribute(String attributeName) {

            requiredAttributes.remove(attributeName);
        }

        /**
         * Store the XDR encoded data from <code>attributes</code> in
         * <code>params</code>.  This enables the check for all required 
         * attributes for a FOM Platform object.
         *
         * @param attributes map of the attribute name 
         * and encoded data to transfer to params.
         */
        public void update(Map<AttributeRTIHandle, byte[]> attributes) {

            for (Iterator<Map.Entry<AttributeRTIHandle, byte[]>> i =
                    attributes.entrySet().iterator(); i.hasNext();) {

                Map.Entry<AttributeRTIHandle, byte[]> item = i.next();
                AttributeRTIHandle attrHandle = item.getKey();
                byte[] encodedData = item.getValue();
                String attributeName = attrHandle.getName();

                setAttribute(attributeName, encodedData);
           }
        }

        /**
         * Have all the required attributes been set for this parameter?.
         *
         * @return boolean value represents that all required attributes have
         *         been set.
         */
        public boolean isValid() {

            boolean valid = true;
            for (String name : requiredAttributes) {
                if (!attributes.containsKey(name)) {
                    valid = false;
                    break;
                }
            }

            return valid;
        }
    }

    /**
     * Interface method to be called from the ObjectHandler on
     * HeartbeatUpdateRequest messages. Translates OOS spatial data to FOM
     * spatial to prevent stale spatial data (due to dead reckoning) on
     * heartbeats.
     *
     * @param objHandleRTI the obj handle rti
     * @param heartbeatData the heartbeat data
     */
    @Override
    public void updateHeartbeatData(ObjectRTIHandle objHandleRTI,
        AbstractOutgoingRTIMessage heartbeatData) {

        if (debugEnabled) {
            logger.debug("Updating heartbeat data on " + objHandleRTI);
        }

        if (!useDRSpatialData) {

            // Create container for attribute names and the associated data byte
            // arrays to be sent to the RTI.
            Map<AttributeRTIHandle, byte[]> outputData =
                new HashMap<AttributeRTIHandle, byte[]>();

            // Get a class RTI handle for the object
            ObjectClassRTIHandle classHandle = objHandleRTI.getClassHandle();

            // Only need a spatial update
            handleODMPropertyChanged(classHandle, objHandleRTI.getProxy(),
                ODM_HB_SPATIAL_DATA, outputData);

            if (!outputData.isEmpty()) {
                heartbeatSrvc.updateHeartbeatData(
                    (UpdateAttributeValues) heartbeatData, outputData);
            }
        }
    }

    /**
     * Handle necessary conversion of the ODM integer to the FOM.
     *
     * @param intValue the int value
     * @param attrName the attr name
     * @param optional the optional
     * @param classHandle the class handle
     * @param outputData the output data
     */
    protected void handleODMInteger(int intValue, String attrName,
        boolean optional, ObjectClassRTIHandle classHandle,
        Map<AttributeRTIHandle, byte[]> outputData) {

        try {

            byte[] outByte = handleODMInteger(intValue, optional);

            setAttributeInClassHandle(classHandle, attrName, outByte,
                outputData);
        } catch (Exception ex) {

            logger.error("handleODMInteger: IOException during conversion of "
                + attrName, ex);
        }
    }

    /**
     * Handle necessary conversion of the ODM boolean to the FOM.
     *
     * @param boolValue the bool value
     * @param attrName the attr name
     * @param optional the optional
     * @param classHandle the class handle
     * @param outputData the output data
     */
    protected void handleODMBoolean(boolean boolValue, String attrName,
        boolean optional, ObjectClassRTIHandle classHandle,
        Map<AttributeRTIHandle, byte[]> outputData) {

        try {

            byte[] outByte = handleODMBoolean(boolValue, optional);
            setAttributeInClassHandle(classHandle, attrName, outByte,
                outputData);
        } catch (Exception ex) {

            logger.error("handleODMBoolean: IOException during conversion of "
                + attrName, ex);
        }
    }

    /**
     * Handle necessary conversion of the ODM short to int.
     *
     * @param shortValue the short value
     * @param attrName the attr name
     * @param optional the optional
     * @param classHandle the class handle
     * @param outputData the output data
     */
    protected void handleODMShortToInt(short shortValue, String attrName,
        boolean optional, ObjectClassRTIHandle classHandle,
        Map<AttributeRTIHandle, byte[]> outputData) {

        try {

            byte[] outByte = handleODMShortToInt(shortValue, optional);
            setAttributeInClassHandle(classHandle, attrName, outByte,
                outputData);
        } catch (Exception ex) {

            logger.error(
                "handleODMShortToInt: IOException during conversion of "
                    + attrName, ex);
        }
    }

    /**
     * Handle necessary conversion of the ODM float.
     *
     * @param floatValue the float value
     * @param attrName the attr name
     * @param optional the optional
     * @param classHandle the class handle
     * @param outputData the output data
     */
    protected void handleODMFloat(float floatValue, String attrName,
        boolean optional, ObjectClassRTIHandle classHandle,
        Map<AttributeRTIHandle, byte[]> outputData) {

        try {

            byte[] outByte = handleODMFloat(floatValue, optional);
            setAttributeInClassHandle(classHandle, attrName, outByte,
                outputData);
        } catch (Exception ex) {

            logger.error("handleODMFloat: IOException during conversion of "
                + attrName, ex);
        }
    }

    /**
     * Handle necessary conversion of the ODM double.
     *
     * @param dblValue the dbl value
     * @param attrName the attr name
     * @param optional the optional
     * @param classHandle the class handle
     * @param outputData the output data
     */
    protected void handleODMDouble(double dblValue, String attrName,
        boolean optional, ObjectClassRTIHandle classHandle,
        Map<AttributeRTIHandle, byte[]> outputData) {

        try {
            byte[] outByte = handleODMDouble(dblValue, optional);
            setAttributeInClassHandle(classHandle, attrName, outByte,
                outputData);
        } catch (Exception ex) {

            logger.error("handleODMDouble: IOException during conversion of "
                + attrName, ex);
        }
    }

    /**
     * Handle necessary conversion of the ODM short.
     *
     * @param shortValue the short value
     * @param attrName the attr name
     * @param optional the optional
     * @param classHandle the class handle
     * @param outputData the output data
     */
    protected void handleODMShort(short shortValue, String attrName,
        boolean optional, ObjectClassRTIHandle classHandle,
        Map<AttributeRTIHandle, byte[]> outputData) {

        try {
            byte[] outByte = handleODMShort(shortValue, optional);
            setAttributeInClassHandle(classHandle, attrName, outByte,
                outputData);
        } catch (Exception ex) {

            logger.error("handleODMShort: IOException during conversion of "
                + attrName, ex);
        }
    }

    /**
     * Handle necessary conversion of the ODM long to the FOM.
     *
     * @param longValue the long value
     * @param attrName the attr name
     * @param optional the optional
     * @param classHandle the class handle
     * @param outputData the output data
     */
    protected void handleODMLong(long longValue, String attrName,
        boolean optional, ObjectClassRTIHandle classHandle,
        Map<AttributeRTIHandle, byte[]> outputData) {

        try {

            byte[] outByte = handleODMLong(longValue, optional);
            setAttributeInClassHandle(classHandle, attrName, outByte,
                outputData);
        } catch (Exception ex) {

            logger.error("handleODMLong: IOException during conversion of "
                + attrName, ex);
        }
    }


    /**
     * Handle necessary conversion of the ODM string to the FOM.
     *
     * @param string the string
     * @param attrName the attr name
     * @param optional the optional
     * @param classHandle the class handle
     * @param outputData the output data
     */
    protected void handleODMString(String string, String attrName,
        boolean optional, ObjectClassRTIHandle classHandle,
        Map<AttributeRTIHandle, byte[]> outputData) {

        if (string != null) {

            try {

                byte[] outByte = handleODMString(string, optional);
                setAttributeInClassHandle(classHandle, attrName, outByte,
                    outputData);
            } catch (Exception ex) {

                logger.error(
                    "handleODMString: IOException during conversion of "
                        + attrName, ex);
            }
        }
    }

    /**
     * Handle necessary conversion of the ODM GCC location to the FOM.
     *
     * @param location the location
     * @param attrName the attr name
     * @param classHandle the class handle
     * @param outputData the output data
     */
    protected void handleODMLocation(GCC location, String attrName,
        ObjectClassRTIHandle classHandle,
        Map<AttributeRTIHandle, byte[]> outputData) {

        if (location != null) {

            try {

                byte[] outByte = handleODMLocation(location);
                setAttributeInClassHandle(classHandle, attrName, outByte,
                    outputData);
            } catch (Exception ex) {

                logger.error(
                    "handleODMLocation: IOException during conversion of "
                        + attrName, ex);
            }
        }
    }

    /**
     * Handle necessary conversion of the ODM identifier to the FOM.
     *
     * @param uid the uid
     * @param attrName the attr name
     * @param classHandle the class handle
     * @param outputData the output data
     */
    protected void handleODMEventID(UniqueID uid, String attrName,
        ObjectClassRTIHandle classHandle,
        Map<AttributeRTIHandle, byte[]> outputData) {

        if (uid != null) {

            try {

                byte[] outByte = handleODMEventID(uid);
                setAttributeInClassHandle(classHandle, attrName, outByte,
                    outputData);
            } catch (Exception ex) {

                logger.error(
                    "handleODMEventID: IOException during conversion of "
                        + attrName, ex);
            }
        }
    }

    /**
     * Handle necessary conversion of the ODM event time to the FOM.
     *
     * @param time the time
     * @param attrName the attr name
     * @param classHandle the class handle
     * @param outputData the output data
     */
    protected void handleODMEventTime(long time, String attrName,
        ObjectClassRTIHandle classHandle,
        Map<AttributeRTIHandle, byte[]> outputData) {

        if (debugEnabled) {
            logger.debug("handling OOS time");
        }

        try {

            byte[] outByte = handleODMEventTime(time);
            setAttributeInClassHandle(classHandle, attrName, outByte,
                outputData);
        } catch (Exception ex) {

            logger.error(
                "handleODMEventTime: IOException during conversion of "
                    + attrName, ex);
        }
    }

    /**
     * Handle necessary conversion of the ODM uid to the FOM.
     *
     * @param uid the uid
     * @param attrName the attr name
     * @param optional the optional
     * @param classHandle the class handle
     * @param outputData the output data
     */
    protected void handleODMID(UniqueID uid, String attrName, boolean optional,
        ObjectClassRTIHandle classHandle,
        Map<AttributeRTIHandle, byte[]> outputData) {

        if (uid != null) {

            try {

                byte[] outByte = handleODMID(uid, optional);
                setAttributeInClassHandle(classHandle, attrName, outByte,
                    outputData);
            } catch (Exception ex) {

                logger.error("handleODMID: IOException during conversion of "
                    + attrName, ex);
            }
        }
    }

    /**
     * Handle necessary conversion of the ODM target number to the FOM.
     *
     * @param targetNumber the target number
     * @param attrName the attr name
     * @param classHandle the class handle
     * @param outputData the output data
     */
    protected void handleODMTargetNumber(String targetNumber, String attrName,
        ObjectClassRTIHandle classHandle,
        Map<AttributeRTIHandle, byte[]> outputData) {

        try {

            byte[] outByte = handleODMTargetNumber(targetNumber);
            setAttributeInClassHandle(classHandle, attrName, outByte,
                outputData);
        } catch (Exception ex) {

            logger.error(
                "handleODMTargetNumber: IOException during conversion of "
                    + attrName, ex);
        }
    }

    /**
     * Handle necessary conversion of the ODM warhead type to the FOM.
     *
     * @param whType the wh type
     * @param attrName the attr name
     * @param classHandle the class handle
     * @param outputData the output data
     */
    protected void handleODMWarheadType(WarheadTypeEnum whType,
        String attrName, ObjectClassRTIHandle classHandle,
        Map<AttributeRTIHandle, byte[]> outputData) {

        if (whType != null) {

            try {

                byte[] outByte = handleODMWarheadType(whType);
                setAttributeInClassHandle(classHandle, attrName, outByte,
                    outputData);
            } catch (Exception ex) {

                logger.error(
                    "handleODMWarheadType: IOException during conversion of "
                        + attrName, ex);
            }
        }
    }

    /**
     * Handle necessary conversion of the ODM munition type to the FOM.
     *
     * @param mType the m type
     * @param attrName the attr name
     * @param classHandle the class handle
     * @param outputData the output data
     */
    protected void handleODMMunitionType(MunitionTypeEnum mType,
        String attrName, ObjectClassRTIHandle classHandle,
        Map<AttributeRTIHandle, byte[]> outputData) {

        if (mType != null) {

            try {

                byte[] outByte = handleODMMunitionType(mType);
                setAttributeInClassHandle(classHandle, attrName, outByte,
                    outputData);
            } catch (Exception ex) {

                logger.error(
                    "handleODMMunitionType: IOException during conversion of "
                        + attrName, ex);
            }
        }
    }

    /**
     * Handle necessary conversion of the ODM munition id to the FOM.
     *
     * @param mID the m id
     * @param attrName the attr name
     * @param classHandle the class handle
     * @param outputData the output data
     */
    protected void handleODMMunitionID(UniqueID mID, String attrName,
        ObjectClassRTIHandle classHandle,
        Map<AttributeRTIHandle, byte[]> outputData) {

        if (mID != null) {

            try {

                byte[] outByte = handleODMMunitionID(mID);
                setAttributeInClassHandle(classHandle, attrName, outByte,
                    outputData);
            } catch (Exception ex) {

                logger.error(
                    "handleODMMunitionID: IOException during conversion of "
                        + attrName, ex);
            }
        }
    }

    /**
     * Handle necessary conversion of the ODM vecotr3d to the FOM.
     *
     * @param velVector the vel vector
     * @param attrName the attr name
     * @param classHandle the class handle
     * @param outputData the output data
     */
    protected void handleODMVector3d(Vector3d velVector, String attrName,
        ObjectClassRTIHandle classHandle,
        Map<AttributeRTIHandle, byte[]> outputData) {

        try {

            byte[] outByte = handleODMVector3d(velVector);
            setAttributeInClassHandle(classHandle, attrName, outByte,
                outputData);
        } catch (Exception ex) {

            logger.error("handleODMVector3d: IOException during conversion of "
                + attrName, ex);
        }
    }

    /**
     * Handle necessary conversion of the ODM fuse type to the FOM.
     *
     * @param fType the f type
     * @param attrName the attr name
     * @param classHandle the class handle
     * @param outputData the output data
     */
    protected void handleODMFuzeType(FuzeTypeEnum fType, String attrName,
        ObjectClassRTIHandle classHandle,
        Map<AttributeRTIHandle, byte[]> outputData) {

        if (fType != null) {

            try {

                byte[] outByte = handleODMFuzeType(fType);
                setAttributeInClassHandle(classHandle, attrName, outByte,
                    outputData);
            } catch (IOException ex) {

                logger.error(
                    "handleODMFuzeType: IOException during conversion of "
                        + attrName, ex);
                return;
            }
        }
    }

    /**
     * Handle necessary conversion of the ODM calendar to the FOM.
     *
     * @param cal the cal
     * @param attrName the attr name
     * @param classHandle the class handle
     * @param outputData the output data
     */
    protected void handleODMCalendar(Calendar cal, String attrName,
        ObjectClassRTIHandle classHandle,
        Map<AttributeRTIHandle, byte[]> outputData) {

        if (cal != null) {

            try {
                byte[] outByte = handleODMCalendar(cal);
                setAttributeInClassHandle(classHandle, attrName, outByte,
                    outputData);
            } catch (IOException ex) {

                logger.error("handleODMCalendar: IOException during "
                    + "conversion of " + attrName, ex);
                return;
            }
        }
    }

    /**
     * Handle necessary conversion of multiple ODM uids to the FOM.
     *
     * @param uids the uids
     * @param attrName the attr name
     * @param classHandle the class handle
     * @param outputData the output data
     */
    protected void handleODMMultipleID(UniqueID[] uids, String attrName,
        ObjectClassRTIHandle classHandle,
        Map<AttributeRTIHandle, byte[]> outputData) {

        if ((uids != null) && (uids.length > 0)) {

            if (debugEnabled) {

                logger.debug("handling ODM multiple ID");
            }

            // Streams used for byte[] creation.
            ByteArrayOutputStream outByte = new ByteArrayOutputStream();
            XDROutputStream outXDR = new XDROutputStream(outByte);

            try {

                outXDR.writeInt(uids.length);

                for (int i = 0; i < uids.length; i++) {

                    String id =
                        mtxSimulationServices.getMATREXIDFromUID(uids[i]);

                    if (id != null) {

                        outXDR.writeString(id, false);
                        setAttributeInClassHandle(classHandle, attrName,
                            outByte.toByteArray(), outputData);
                    }
                }
            } catch (IOException e) {

                logger.error("handleODMMultipleID:  IOException during "
                    + "conversion of " + attrName, e);
            }
        }
    }
    
    /**
     * Handle necessary conversion of the actor ActorOwnershipEnum to the FOM.
     * @param ownershipVal the ActorOwnershipEnum value
     * @param classHandle the class handle 
     * @param outputData the HLA output data
     */
    protected void handleODMActorOwnership(
        ActorOwnershipEnum ownershipVal,
        ObjectClassRTIHandle classHandle,
        Map<AttributeRTIHandle, byte[]> outputData) {

        if (ownershipVal != null) {

            handleODMInteger(
                ownershipVal.getValue(), 
                FOM_ACTOR_OWNERSHIP, 
                true, 
                classHandle, 
                outputData);

        }
    }
    
    /**
     * XDR decode actor ownership attribute from <code>encodedData</code>. It will be
     * translated to an OneSAF ActorOwnershipEnum and set on <code>odmActor</code>.
     * 
     * @param odmActor the actor to have the translate ActorOwnershipEnum set.
     * @param encodedData byte[] XDR encoded data.
     */
    protected void handleFOMActorOwnership(AbstractActor odmActor, byte[] encodedData) {

        if (debugEnabled) {
            logger.debug("handling FOM " + FOM_ACTOR_OWNERSHIP);
        }

        int value = handleFOMInteger(true, encodedData);
        ActorOwnershipEnum newVal = ActorOwnershipEnum.valueOf(value);
        
        if (newVal != odmActor.getActorOwnership()) {
            if (newVal == ActorOwnershipEnum.AVAILABLE_TO_RECEIVE){
                AlertParameters alert = new AlertParameters();
                alert.problem = Localization.convert(RECEIVED_AVAILBLE_FOR_SUBORDINATE, odmActor.getName());
                alert.resolution = Localization.convert(AVAILBLE_FOR_SUBORDINATE_INSTRUCTION, odmActor.getName());
                alert.severity = AlertLevel.INFO;
                alert.source = hlaManager.getHLAInterface().getName();
                alert.actorName = odmActor.getName();

                AlertManager.getInstance().publishAlert(alert);
            }
            
            odmActor.setActorOwnership(ActorOwnershipEnum.valueOf(value)); 
        }
    }
}
