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
import java.util.Calendar;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import javax.vecmath.Vector3d;

import net.onesaf.core.services.data.dm.objects.common.LongMutable;
import net.onesaf.core.services.data.dm.objects.ecs.coordinate.GCC;
import net.onesaf.core.services.data.dm.objects.model.FuzeTypeEnum;
import net.onesaf.core.services.data.wrappers.common.OOSInteropMapper;
import net.onesaf.core.services.data.wrappers.common.OOSInteropMapperManager;
import net.onesaf.core.services.odm.ODM;
import net.onesaf.core.services.odm.cm.ControlMeasureManager;
import net.onesaf.core.services.odm.emission.EmissionManager;
import net.onesaf.core.services.odm.entity.EntityManager;
import net.onesaf.core.services.odm.eom.EnvironmentManager;
import net.onesaf.core.services.odm.magic.MagicServices;
import net.onesaf.core.services.odm.unit.UnitManager;
import net.onesaf.core.services.sim.interopmgt.InteropProperties;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.TranslationManager;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.amb.OOSAmbassador;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.amb.RTIAmbassadorException;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.fedmgt.FederationData;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.fedmgt.FederateStatus.FederateStatusEnum;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.rtihandle.AttributeRTIHandle;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.rtihandle.InteractionClassRTIHandle;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.rtihandle.ObjectClassRTIHandle;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.rtihandle.ObjectRTIHandle;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.rtihandle.ParameterRTIHandle;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.rtihandle.RTIHandleManager;
import net.onesaf.core.services.sim.interopmgt.hla.common.stream.xdr.XDROutputStream;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.HLA13Manager;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.fom.matrex.converter.util.ABCAidConverter;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.fom.matrex.converter.util.DateTimeGroupToCalendarConverter;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.fom.matrex.converter.util.EnumTranslator;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.fom.matrex.converter.util.FederateID;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.fom.matrex.converter.util.FederateIDConverter;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.fom.matrex.converter.util.MATREXSimulationServices;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.fom.matrex.converter.util.RadianGdcToGccConverter;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.fom.matrex.converter.util.RemoteCreateProcessor;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.fom.matrex.converter.util.SimulationTimeToNTPConverter;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.fom.matrex.converter.util.SpatialStructToStateVectorConverter;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.fom.matrex.converter.util.VelocityWorldXYZConverter;
import net.onesaf.core.services.sim.simeng.timemgt.TimeManager;
import net.onesaf.core.services.sys.bso.MunitionTypeEnum;
import net.onesaf.core.services.sys.bso.WarheadTypeEnum;
import net.onesaf.core.services.sys.composition.ComponentManager;
import net.onesaf.core.services.sys.id.UniqueID;
import net.onesaf.core.services.sys.id.UniqueIDGenerator;
import net.onesaf.core.services.sys.logger2.LevelChangeListener;
import net.onesaf.core.services.sys.logger2.OneSAFLogger;
import net.onesaf.core.services.sys.odb.ObjectDatabase;

import org.apache.log4j.Level;

/**
 * Top parent class for all MATREX converters. This class stores class
 * references, default data and a data method used by most converters.
 *
 * @author $Author$
 * @version $Revision$
 */
public abstract class AbstractConverter {

    /** Reference to the OneSAFLogger object. */
    private static OneSAFLogger logger = (OneSAFLogger) OneSAFLogger
        .getLogger(AbstractConverter.class);

    /** The "Timestamp" FOM attribute. */
    protected static final String FOM_TIMESTAMP = "Timestamp";

    /**  Is debug logging enabled?. */
    protected static boolean debugEnabled = logger.isEnabledFor(Level.DEBUG);

    /**  Is warn logging enabled?. */
    protected static boolean warnEnabled = logger.isEnabledFor(Level.WARN);

    /**  Is info logging enabled?. */
    protected static boolean infoEnabled = logger.isEnabledFor(Level.INFO);

    // initialize the logger levels
    static {

        logger.addLevelChangeListener(new LevelChangeListener() {
            @Override
            public void levelChanged(Level oldLevel, Level newLevel) {

                debugEnabled = logger.isEnabledFor(Level.DEBUG);
                warnEnabled = logger.isEnabledFor(Level.WARN);
                infoEnabled = logger.isEnabledFor(Level.INFO);
            }
        });
    }

    /**
     * Reference to the EntityManager object.
     */
    protected static EntityManager entityManager = null;
    
    /**
     * Reference to the EmissionManager object.
     */
    protected static EmissionManager emissionManager = null;

    /**
     * Reference to the UnitManager object.
     */
    protected static UnitManager unitManager = null;

    /** Reference to the time manager. Stored for use by subclasses. */
    protected static TimeManager timeManager = null;

    /** Instance of the InteropProperties. */
    protected static InteropProperties interopProps = null;

    /** Indicates whether or not ground clamping is enabled. */
    protected static boolean groundClamping = false;

    /**
     * Reference to the MATREXSimulationServices object. Used or for federation
     * string operations as well as other utility functions.
     */
    protected static MATREXSimulationServices mtxSimulationServices = null;

    /**
     * Reference to the enumeration translator. This class is the bridge between
     * OOS enumerations and MATREX FOM enumerations.
     */
    protected static EnumTranslator enumTranslator = null;

    /**
     * Reference to the magic services. Useful for functions such as magic
     * mount.
     */
    protected static MagicServices magicServices = null;

    /**
     * Provides a mapping of FOM entity type to OOS compositions. The mapping
     * file is defined in the interopConfig file.
     */
    protected static OOSInteropMapper entityMapper = null;

    /**
     * Provides a mapping of FOM individual combatant type to OOS compositions.
     * The mapping file is defined in the interopConfig file.
     */
    protected static OOSInteropMapper lifeformMapper = null;

    /**
     * Provides a mapping of FOM munition type to OOS compositions. The mapping
     * file is defined in the interopConfig file. This is limited to the few
     * munitions that are represented as FOM objects.
     */
    protected static OOSInteropMapper munitionMapper = null;

    /**
     * Provides a mapping of FOM UGS type to OOS compositions. The mapping file
     * is defined in the interopConfig file.
     */
    protected static OOSInteropMapper ugsMapper = null;

    /** Reference to the unique id generator. Stored for use by subclasses. */
    protected static UniqueIDGenerator uniqueIDGenerator = null;

    /** Reference to the object database. Stored for use by subclasses. */
    protected static ObjectDatabase objectDatabase = null;

    /** Reference to the hla interrop manager. Stored for use by subclasses. */
    protected static HLA13Manager hlaManager = null;

    /** Reference to the RTI Handle Manager. */
    protected static RTIHandleManager rtiHandleMngr;

    /** Reference to the environment manager. Stored for use by subclasses. */
    protected static EnvironmentManager environmentManager = null;

    /**
     * Reference to the control measure manager. Stored for use by subclasses.
     */
    protected static ControlMeasureManager controlMeasureManager = null;

    /** Reference to the oos amabassador. Stored for use by subclasses. */
    protected static OOSAmbassador oosAmbassador = null;

    /** Reference to the odm. Stored for use by subclasses. */
    protected static ODM odm = null;

    /** Reference to the RemoteCreateProcessor instance. */
    private static RemoteCreateProcessor remoteCreateProcessor = null;

    /**
     * The default ground platform composition. This is defined in the
     * interopConfig file and is populated by AbstractObjectConverter. It is
     * stored at this level because it needs to be accessible to more than
     * object converters.
     */
    protected static String DEFAULT_GROUND_PLATFORM_COMPOSITION = null;

    /**
     * The default aircraft composition. This is defined in the interopConfig
     * file and is populated by AbstractObjectConverter. It is stored at this
     * level because it needs to be accessible to more than object converters.
     */
    protected static String DEFAULT_AIRCRAFT_COMPOSITION = null;

    /**
     * The default IC composition. This is defined in the interopConfig file and
     * is populated by AbstractObjectConverter. It is stored at this level
     * because it needs to be accessible to more than object converters.
     */
    protected static String DEFAULT_IC_COMPOSITION = null;

    /**
     * The default munition composition. This is defined in the interopConfig
     * file and is populated by AbstractObjectConverter. It is stored at this
     * level because it needs to be accessible to more than object converters.
     */
    protected static String DEFAULT_MUNITION_COMPOSITION = null;

    /**
     * The default unit composition. This is defined in the interopConfig file
     * and is populated by AbstractObjectConverter. It is stored at this level
     * because it needs to be accessible to more than object converters.
     */
    protected static String DEFAULT_UNIT_COMPOSITION = null;

    /**
     * Platform enumeration type. This is defined in the interopConfig file and
     * is populated by AbstractObjectConverter. It is stored at this level
     * because it needs to be accessible to more than object converters.
     */
    protected static String PLATFORM_ENUM = null;

    /**
     * IC enumeration type. This is defined in the interopConfig file and is
     * populated by AbstractObjectConverter. It is stored at this level because
     * it needs to be accessible to more than object converters.
     */
    protected static String IC_ENUM = null;

    /**
     * ArticulatedParts Setting enumeration type. This is defined in the
     * interopConfig file and is populated by AbstractObjectConverter. It is
     * stored at this level because it needs to be accessible to more than
     * object converters.
     */
    protected static String ARTICULATED_PARTS_SETTING = null;

    /**
     * Munition enumeration type. This is defined in the interopConfig file and
     * is populated by AbstractObjectConverter. It is stored at this level
     * because it needs to be accessible to more than object converters.
     */
    protected static String MUNITION_ENUM = null;

    /**
     * Set of all diuscovered FOM objects. Objects should be added upon discover
     * and removed when deleted.
     */
    protected static Set<ObjectRTIHandle> discoveredFOMObjects = null;

    /** 
     * Whether messages should be deliberately created with defects,
     * to test other federate's ability to handle them
     */
    private static boolean createDefectiveMessages = false;
    
    /**
     * Initializes the references stored in this class. Will only allow
     * initialization once. The hlaManager is used as a check to prevent
     * multiple initializations.
     *
     * @param transManager the trans manager
     */
    protected void initialize(TranslationManager transManager) {

        if (hlaManager == null) {

            OOSInteropMapperManager interopMapperManager =
                OOSInteropMapperManager.getInstance();

            odm = (ODM) ComponentManager.getInstance(ODM.class.getName());
            entityManager = odm.getEntityManager();
            emissionManager= odm.getEmissionManager();
            unitManager = odm.getUnitManager();
            environmentManager = odm.getEnvironmentManager();
            controlMeasureManager = odm.getControlMeasureManager();

            hlaManager = (HLA13Manager) transManager.getHLAManager();
            rtiHandleMngr = hlaManager.getRTIHandleManager();

            unitManager = odm.getUnitManager();
            timeManager = TimeManager.getInstance();
            interopMapperManager = OOSInteropMapperManager.getInstance();

            interopProps = InteropProperties.getInstance();
            groundClamping = interopProps.isGroundClampEnabled();

            createDefectiveMessages = interopProps.isCreateDefectiveMessages();
            if (createDefectiveMessages) {
                logger.errorOnce("AbstractConverter: WILL CREATE DEFECTIVE HLA MESSAGES");
            }
            
            mtxSimulationServices = MATREXSimulationServices.getInstance();
            mtxSimulationServices.initialize(hlaManager);
            mtxSimulationServices.setDetGroundClampMethod(hlaManager
                .getConfig().DetGroundClampEnum);

            enumTranslator = EnumTranslator.getInstance();
            enumTranslator.initialize(hlaManager.getConfig().FOMName);
            magicServices = MagicServices.getInstance();
            uniqueIDGenerator = UniqueIDGenerator.getInstance();
            objectDatabase =
                (ObjectDatabase) ComponentManager
                    .getInstance(ObjectDatabase.class.getName());
            oosAmbassador = hlaManager.getOOSAmbassador();
            discoveredFOMObjects = new HashSet<ObjectRTIHandle>();

            SpatialStructToStateVectorConverter.setHLAManager(hlaManager);

            if (debugEnabled) {

                logger.debug("publishWallclockTimeStamps is set to true.  "
                    + "All Timestamps will be published as Wallclock time.");
            }
            
            // Get name of entity mapper file from configuration setting
            entityMapper =
                interopMapperManager.getOOSInteropMapper("ENTITY_MAP");

            if (entityMapper != null) {

                // Provide access to the entity map for display in the ICT
                hlaManager.getHLAInterface().setPlatformCompositionTranslator(
                    entityMapper);
            }

            // Get name of lifeform mapper file from configuration setting
            lifeformMapper =
                interopMapperManager.getOOSInteropMapper("LIFEFORM_MAP");

            if (lifeformMapper != null) {

                // Provide access to the lifeform map for display in the ICT
                hlaManager.getHLAInterface().setLifeformCompositionTranslator(
                    lifeformMapper);
            }

            // Get name of munition mapper file from configuration setting
            munitionMapper =
                interopMapperManager.getOOSInteropMapper("MUNITION_MAP");

            if (munitionMapper != null) {

                // Provide access to the munition map for display in the ICT
                hlaManager.getHLAInterface().setMunitionTypeTranslator(
                    munitionMapper);
            }

            // Get name of ugs mapper file from configuration setting
            ugsMapper = interopMapperManager.getOOSInteropMapper("UGS_MAP");

            if (ugsMapper != null) {

                // need to add function to translationManager to get ugs mapper
            }

        }
        remoteCreateProcessor =
            RemoteCreateProcessor.getInstance(hlaManager, objectDatabase,
                controlMeasureManager);

    }
    
    protected FederationData getFederationData() {
        return hlaManager.getFederationData();
    }

    /**
     * Is the rtiHandle in the set of discovered fom objects? If not, log a
     * warning.
     * 
     * @param rtiHandle Is this handle valid?
     * @return boolean Was rtiHandle within the discovered objects?
     */
    protected boolean validFOMObject(ObjectRTIHandle rtiHandle) {

        boolean valid = discoveredFOMObjects.contains(rtiHandle);

        if (warnEnabled) {

            logger.warn("Received request for unknown object: "
                + rtiHandle.getName());
        }

        return valid;
    }

    /**
     * Helper functions for subclasses. If <code>bytes</code> is valid,
     * retrieves the attribute handle for <code>attrName</code> on
     * <code>classHandle</code> and adds an entry relating the attribute handle
     * to <code>bytes</code> within <code>outputData</code>.
     * 
     * @param classHandle ObjectClassRTIHandle of the class to get the attribute
     *        handle from.
     * @param attrName String of the attribute name to be setting.
     * @param bytes byte[] to be stored in the 'outputData' for the attribute
     * @param outputData Map of attribute handle to byte[] to be returned to the
     *        caller.
     */
    protected void setAttributeInClassHandle(ObjectClassRTIHandle classHandle,
        String attrName, byte[] bytes,
        Map<AttributeRTIHandle, byte[]> outputData) {

        if (bytes != null && bytes.length > 0) {

            // Get RTI handle for updated FOM attribute
            AttributeRTIHandle attrHandle = null;
            try {
                attrHandle =
                    rtiHandleMngr.getAttributeHandle(attrName, classHandle);
            } catch (RTIAmbassadorException rtiEx) {
                logger.error("RTI Error: ", rtiEx);
                return;
            }

            if (debugEnabled) {
                logger.debug("Setting value in attribute: " + attrName
                    + " (RTIHandleValue = " + attrHandle + ")");
            }

            if (createDefectiveMessages) {
                byte[] bytes1 = new byte[2];
                bytes[0] = 9;
                outputData.put(attrHandle, bytes1);
            } else {
                outputData.put(attrHandle, bytes);
            }
        }
    }

    /**
     * If <code>bytes</code> is non-null and non-empty, then get the attribute
     * handle using the <code>classHandle</code> for the specified
     * <code>paramName</code> and add the resulting parameter handle to the
     * <code>outputData</code> map as the key and the <code>bytes</code> as the
     * value.
     * 
     * @param classHandle ObjectClassRTIHandle of the class to get the attribute
     *        handle from.
     * @param paramName String of the parameter name to be setting.
     * @param bytes byte[] to be stored in the 'outputData' for the attribute
     * @param outputData Map of parameter handle to byte[] to be returned to the
     *        caller.
     */
    protected void setParameterInClassHandle(
        InteractionClassRTIHandle classHandle, String paramName, byte[] bytes,
        Map<ParameterRTIHandle, byte[]> outputData) {

        if (bytes != null && bytes.length > 0) {

            // Get RTI handle for updated FOM attribute
            ParameterRTIHandle paramHandle = null;
            try {
                paramHandle =
                    rtiHandleMngr.getParameterHandle(paramName, classHandle);
            } catch (RTIAmbassadorException rtiEx) {
                logger.error("RTI Error: ", rtiEx);
                return;
            }

            if (debugEnabled) {
                logger.debug("Setting value in parameter: " + paramName
                    + " (RTIHandleValue = " + paramHandle + ")");
            }

            if (createDefectiveMessages) {
                byte[] bytes1 = new byte[2];
                bytes[0] = 9;
                outputData.put(paramHandle, bytes1);
            } else {
                outputData.put(paramHandle, bytes);
            }
        }
    }

    // *************************************************************************
    // ************************* ODM handler methods ***************************
    // *************************************************************************

    /**
     * Handle necessary conversion of the ODM integer to the FOM.
     *
     * @param intValue the int value
     * @param optional the optional
     * @return the byte[]
     * @throws IOException Signals that an I/O exception has occurred.
     */
    protected byte[] handleODMInteger(int intValue, boolean optional)
        throws IOException {

        if (debugEnabled) {
            logger.debug("handling OOS integer");
        }

        // Streams used for byte[] creation
        ByteArrayOutputStream outByte = new ByteArrayOutputStream();
        XDROutputStream outXDR = new XDROutputStream(outByte);

        if (optional) {
            outXDR.writeInt(1);
        }

        outXDR.writeInt(intValue);
        outXDR.close();
        return outByte.toByteArray();
    }

    /**
     * Handle necessary conversion of the ODM boolean to the FOM.
     *
     * @param boolValue the bool value
     * @param optional the optional
     * @return the byte[]
     * @throws IOException Signals that an I/O exception has occurred.
     */
    protected byte[] handleODMBoolean(boolean boolValue, boolean optional)
        throws IOException {

        if (debugEnabled) {

            logger.debug("handling OOS boolean");
        }

        // Streams used for byte[] creation
        ByteArrayOutputStream outByte = new ByteArrayOutputStream();
        XDROutputStream outXDR = new XDROutputStream(outByte);

        if (optional) {
            outXDR.writeInt(1);
        }

        outXDR.writeXDRBoolean(boolValue);
        outXDR.close();
        
        return outByte.toByteArray();
    }

    /**
     * Handle necessary conversion of the ODM short to int.
     *
     * @param shortValue the short value
     * @param optional the optional
     * @return the byte[]
     * @throws IOException Signals that an I/O exception has occurred.
     */
    protected byte[] handleODMShortToInt(short shortValue, boolean optional)
        throws IOException {

        if (debugEnabled) {
            logger.debug("handling OOS short to int");
        }

        // Streams used for byte[] creation
        ByteArrayOutputStream outByte = new ByteArrayOutputStream();
        XDROutputStream outXDR = new XDROutputStream(outByte);

        if (optional) {
            outXDR.writeInt(1);
        }

        outXDR.writeInt(shortValue);
        outXDR.close();
        return outByte.toByteArray();
    }

    /**
     * Handle necessary conversion of the ODM float.
     *
     * @param floatValue the float value
     * @param optional the optional
     * @return the byte[]
     * @throws IOException Signals that an I/O exception has occurred.
     */
    protected byte[] handleODMFloat(float floatValue, boolean optional)
        throws IOException {

        if (debugEnabled) {
            logger.debug("handling OOS float");
        }

        // Streams used for byte[] creation
        ByteArrayOutputStream outByte = new ByteArrayOutputStream();
        XDROutputStream outXDR = new XDROutputStream(outByte);

        if (optional) {
            outXDR.writeInt(1);
        }

        outXDR.writeFloat(floatValue);
        outXDR.close();
        return outByte.toByteArray();
    }

    /**
     * Handle necessary conversion of the ODM double.
     *
     * @param dblValue the dbl value
     * @param optional the optional
     * @return the byte[]
     * @throws IOException Signals that an I/O exception has occurred.
     */
    protected byte[] handleODMDouble(double dblValue, boolean optional)
        throws IOException {

        if (debugEnabled) {
            logger.debug("handling OOS double");
        }

        // Streams used for byte[] creation
        ByteArrayOutputStream outByte = new ByteArrayOutputStream();
        XDROutputStream outXDR = new XDROutputStream(outByte);

        if (optional) {
            outXDR.writeInt(1);
        }

        outXDR.writeDouble(dblValue);
        outXDR.close();
        return outByte.toByteArray();
    }

    /**
     * Handle necessary conversion of the ODM short.
     *
     * @param shortValue the short value
     * @param optional the optional
     * @return the byte[]
     * @throws IOException Signals that an I/O exception has occurred.
     */
    protected byte[] handleODMShort(short shortValue, boolean optional)
        throws IOException {

        if (debugEnabled) {
            logger.debug("handling OOS short");
        }

        // Streams used for byte[] creation
        ByteArrayOutputStream outByte = new ByteArrayOutputStream();
        XDROutputStream outXDR = new XDROutputStream(outByte);

        if (optional) {
            outXDR.writeInt(1);
        }

        outXDR.writeShort(shortValue);
        // for XDR purposes need to pad for 4 byte alignment
        outXDR.writeShort(0);
        outXDR.close();
        return outByte.toByteArray();
    }

    /**
     * Handle necessary conversion of the ODM long to the FOM.
     *
     * @param longValue the long value
     * @param optional the optional
     * @return the byte[]
     * @throws IOException Signals that an I/O exception has occurred.
     */
    protected byte[] handleODMLong(long longValue, boolean optional)
        throws IOException {

        if (debugEnabled) {
            logger.debug("handling OOS long");
        }

        // Streams used for byte[] creation
        ByteArrayOutputStream outByte = new ByteArrayOutputStream();
        XDROutputStream outXDR = new XDROutputStream(outByte);

        if (optional) {
            outXDR.writeInt(1);
        }

        outXDR.writeLong(longValue);
        outXDR.close();
        return outByte.toByteArray();
    }

    /**
     * Handle necessary conversion of the ODM string to the FOM.
     *
     * @param string the string
     * @param optional the optional
     * @return the byte[]
     * @throws IOException Signals that an I/O exception has occurred.
     */
    protected byte[] handleODMString(String string, boolean optional)
        throws IOException {

        // Streams used for byte[] creation
        ByteArrayOutputStream outByte = new ByteArrayOutputStream();
        XDROutputStream outXDR = new XDROutputStream(outByte);

        if (string != null) {

            if (debugEnabled) {
                logger.debug("handling OOS string");
            }

            if (optional) {
                outXDR.writeInt(1);
            }

            outXDR.writeString(string, false);
        }
        
        outXDR.close();
        return outByte.toByteArray();
    }

    /**
     * Handle necessary conversion of the ODM GCC location to the FOM.
     *
     * @param location the location
     * @return the byte[]
     * @throws IOException Signals that an I/O exception has occurred.
     */
    protected byte[] handleODMLocation(GCC location) throws IOException {
        return handleODMLocation(location, false);
    }

    /**
     * Handle necessary conversion of the ODM GCC location to the FOM.
     *
     * @param location the location
     * @param optional the optional
     * @return the byte[]
     * @throws IOException Signals that an I/O exception has occurred.
     */
    protected byte[] handleODMLocation(GCC location, boolean optional) throws IOException {

        // Streams used for byte[] creation
        ByteArrayOutputStream outByte = new ByteArrayOutputStream();
        XDROutputStream outXDR = new XDROutputStream(outByte);

        if (location != null) {
            if (optional) {
                outXDR.writeInt(1);
            }
            RadianGdcToGccConverter.writeToFOM(location, outXDR);
        }
        return outByte.toByteArray();
    }

    /**
     * Handle necessary conversion of the ODM identifier to the FOM.
     *
     * @param uid the uid
     * @return the byte[]
     * @throws IOException Signals that an I/O exception has occurred.
     */
    protected byte[] handleODMEventID(UniqueID uid) throws IOException {

        // Streams used for byte[] creation
        ByteArrayOutputStream outByte = new ByteArrayOutputStream();
        XDROutputStream outXDR = new XDROutputStream(outByte);

        if (uid != null) {

            FederateID tmpSID = mtxSimulationServices.getFederateIDFromUID(uid);
            FederateIDConverter.writeToFOM(tmpSID, outXDR);
        }
        return outByte.toByteArray();
    }

    /**
     * Handle necessary conversion of the ODM event time to the FOM.
     *
     * @param time the time
     * @return the byte[]
     * @throws IOException Signals that an I/O exception has occurred.
     */
    protected byte[] handleODMEventTime(long time) throws IOException {

        long pubTime;

        if (hlaManager.getConfig().publishWallclockTimeStamps) {

            pubTime = System.currentTimeMillis();
        } else {

            pubTime = time;
        }

        // Streams used for byte[] creation
        ByteArrayOutputStream outByte = new ByteArrayOutputStream();
        XDROutputStream outXDR = new XDROutputStream(outByte);

        LongMutable lVal = new LongMutable();
        lVal.longValue = pubTime;
        SimulationTimeToNTPConverter.writeToFOM(lVal, outXDR);
        return outByte.toByteArray();
    }

    /**
     * Handle necessary conversion of the ODM uid to the FOM.
     *
     * @param uid the uid
     * @param optional the optional
     * @return the byte[]
     * @throws IOException Signals that an I/O exception has occurred.
     */
    protected byte[] handleODMID(UniqueID uid, boolean optional)
        throws IOException {

        // Streams used for byte[] creation
        ByteArrayOutputStream outByte = new ByteArrayOutputStream();
        XDROutputStream outXDR = new XDROutputStream(outByte);

        if (uid != null) {

            if (debugEnabled) {

                logger.debug("handling OOS uid");
            }

            String id = mtxSimulationServices.getMATREXIDFromUID(uid);

            if (id != null) {

                if (optional) {

                    outXDR.writeInt(1);
                }

                outXDR.writeString(id, false);
            }
        }

        outXDR.close();
        
        return outByte.toByteArray();
    }

    /**
     * Handle necessary conversion of the ODM target number to the FOM.
     *
     * @param targetNumber the target number
     * @return the byte[]
     * @throws IOException Signals that an I/O exception has occurred.
     */
    protected byte[] handleODMTargetNumber(String targetNumber)
        throws IOException {

        if (debugEnabled) {
            logger.debug("handling OOS target number");
        }

        // Streams used for byte[] creation
        ByteArrayOutputStream outByte = new ByteArrayOutputStream();
        XDROutputStream outXDR = new XDROutputStream(outByte);

        StringBuffer sBuf = new StringBuffer(targetNumber);
        ABCAidConverter.writeToFOM(sBuf, outXDR);

        return outByte.toByteArray();
    }

    /**
     * Handle necessary conversion of the ODM warhead type to the FOM.
     *
     * @param whType the wh type
     * @return the byte[]
     * @throws IOException Signals that an I/O exception has occurred.
     */
    protected byte[] handleODMWarheadType(WarheadTypeEnum whType)
        throws IOException {

        // Streams used for byte[] creation
        ByteArrayOutputStream outByte = new ByteArrayOutputStream();
        XDROutputStream outXDR = new XDROutputStream(outByte);

        if (whType != null) {

            if (debugEnabled) {

                logger.debug("handling OOS warhead type");
            }

            outXDR.writeInt(enumTranslator.getWarheadType(whType));
        }
        
        outXDR.close();
        
        return outByte.toByteArray();
    }

    /**
     * Handle necessary conversion of the ODM munition type to the FOM.
     *
     * @param mType the m type
     * @return the byte[]
     * @throws IOException Signals that an I/O exception has occurred.
     */
    protected byte[] handleODMMunitionType(MunitionTypeEnum mType)
        throws IOException {

        // Streams used for byte[] creation
        ByteArrayOutputStream outByte = new ByteArrayOutputStream();
        XDROutputStream outXDR = new XDROutputStream(outByte);

        if (mType != null) {
            Integer munition = null;

            if (debugEnabled) {

                logger.debug("handling OOS munition type");
            }

            munition = enumTranslator.getMunitionType(mType);

            if (munition != null) {

                outXDR.writeInt(munition);

                if (munition.intValue() == 0) {
                    logger
                        .error("MunitionType conversion error : OOS-->HLA : Cannot find mapping for OOS munition "
                            + mType);
                }
            }
        }
        
        outXDR.close();
        
        return outByte.toByteArray();
    }

    /**
     * Handle necessary conversion of the ODM munition type to the FOM.
     *
     * @param mType the m type
     * @return the byte[]
     * @throws IOException Signals that an I/O exception has occurred.
     */
    protected byte[] handleODMDISMunitionType(MunitionTypeEnum mType)
        throws IOException {

    	// Streams used for byte[] creation
		ByteArrayOutputStream outByte = new ByteArrayOutputStream();
		XDROutputStream outXDR = new XDROutputStream(outByte);

		if (mType != null) {
			String munition = null;

			if (debugEnabled) {

				logger.debug("handling OOS munition type");
			}

			munition = enumTranslator.getDISMunitionType(mType);

			if (munition == null || munition.equals("")) {

				logger.error("MunitionType conversion error : OOS-->HLA : Cannot find mapping for OOS munition "
						+ mType + ". Defaulting to 0.0.0.0.0.0.0");
				munition = "0.0.0.0.0.0.0";
			}

			// Split the DISMunitionType external identifier and assign to
			// individual elements
			String[] eTypeElements = munition.split("\\.");
			if (eTypeElements != null && eTypeElements.length < 7) {
				logger.error("externalId is: " + munition
						+ " eTypeElements is of size: " + eTypeElements.length
						+ " but should be of size 7");
			}

			int kind = Integer.parseInt(eTypeElements[0]);
			int domain = Integer.parseInt(eTypeElements[1]);
			int countryCode = Integer.parseInt(eTypeElements[2]);
			int category = Integer.parseInt(eTypeElements[3]);
			int subcategory = Integer.parseInt(eTypeElements[4]);
			int specific = Integer.parseInt(eTypeElements[5]);
			int extra = Integer.parseInt(eTypeElements[6]);

			// writing optional field
			outXDR.writeInt(1);
			outXDR.writeInt(kind);
			outXDR.writeInt(domain);
			outXDR.writeInt(countryCode);
			outXDR.writeInt(category);
			outXDR.writeInt(subcategory);
			outXDR.writeInt(specific);
			outXDR.writeInt(extra);
		}
		
		outXDR.close();
		
		return outByte.toByteArray();
    }

    /**
     * Handle necessary conversion of the ODM munition id to the FOM.
     *
     * @param mID the m id
     * @return the byte[]
     * @throws IOException Signals that an I/O exception has occurred.
     */
    protected byte[] handleODMMunitionID(UniqueID mID) throws IOException {

        // Streams used for byte[] creation
        ByteArrayOutputStream outByte = new ByteArrayOutputStream();
        XDROutputStream outXDR = new XDROutputStream(outByte);

        if (mID != null) {

            if (debugEnabled) {

                logger.debug("handling OOS munition id");
            }

            String newID = mtxSimulationServices.getNextUniqueMunitionName();
            outXDR.writeString(newID, false);
        }
        
        outXDR.close();
        
        return outByte.toByteArray();
    }

    /**
     * Handle necessary conversion of the ODM vecotr3d to the FOM.
     *
     * @param velVector the vel vector
     * @return the byte[]
     * @throws IOException Signals that an I/O exception has occurred.
     */
    protected byte[] handleODMVector3d(Vector3d velVector) throws IOException {

        if (debugEnabled) {
            logger.debug("handling OOS vector 3d");
        }

        // Streams used for byte[] creation
        ByteArrayOutputStream outByte = new ByteArrayOutputStream();
        XDROutputStream outXDR = new XDROutputStream(outByte);

        VelocityWorldXYZConverter.writeToFOM(velVector, outXDR);
        return outByte.toByteArray();
    }

    /**
     * Handle necessary conversion of the ODM fuse type to the FOM.
     *
     * @param fType the f type
     * @return the byte[]
     * @throws IOException Signals that an I/O exception has occurred.
     */
    protected byte[] handleODMFuzeType(FuzeTypeEnum fType) throws IOException {

        if (debugEnabled) {
            logger.debug("handling OOS fuze type");
        }

        // Streams used for byte[] creation
        ByteArrayOutputStream outByte = new ByteArrayOutputStream();
        XDROutputStream outXDR = new XDROutputStream(outByte);

        if (fType != null) {

            outXDR.writeInt(enumTranslator.getFuzeType(fType).intValue());
        }
        
        outXDR.close();
        
        return outByte.toByteArray();
    }

    /**
     * Handle necessary conversion of the ODM calendar to the FOM.
     *
     * @param cal the cal
     * @return the byte[]
     * @throws IOException Signals that an I/O exception has occurred.
     */
    protected byte[] handleODMCalendar(Calendar cal) throws IOException {

        if (debugEnabled) {
            logger.debug("handling OOS calendar");
        }

        // Streams used for byte[] creation
        ByteArrayOutputStream outByte = new ByteArrayOutputStream();
        XDROutputStream outXDR = new XDROutputStream(outByte);

        if (cal != null) {
            DateTimeGroupToCalendarConverter.writeToFOM(cal, outXDR);
        }
        return outByte.toByteArray();
    }

    /**
     * Posts a message on the HLAInterface; useful for communicating to the ICT
     * message pane.
     *
     * @param msg the message
     */
    protected final void postMessage(String msg) {
        if (hlaManager != null && msg != null) {
            hlaManager.getHLAInterface().fireMessage(msg);
        }
    }

    /**
     * Posts a message on the HLAInterface; useful for communicating to the ICT
     * message pane.  Repeated msg strings are ignored.
     *
     * @param msg the message
     */
    protected final void postMessageOnce(String msg) {
        if (hlaManager != null && msg != null) {
            hlaManager.getHLAInterface().fireMessageOnce(msg);
        }
    }
    
    /**
     * Handle fed status change.
     *
     * @param fedStatus the fed status
     */
    protected void handleFedStatusChange(FederateStatusEnum fedStatus) {

        if (fedStatus == FederateStatusEnum.PRE_RESIGN) {
            synchronized (MATREXSimulationServices.getInstance().mountMap) {
                mtxSimulationServices.clearEntityIDMap();
                MATREXSimulationServices.getInstance().mountMap.clear();
            }
        }
    }
}
