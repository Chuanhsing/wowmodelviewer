/*
 3D Model Exporter Server

 Exporting is now done with plugin files. This means anyone can write and
 distribute an exporter without needing to upload the code to WMV's code.
*/

#ifndef WMVENGINE_MODEL_EXPORTER_SERVER_H
#define WMVENGINE_MODEL_EXPORTER_SERVER_H

#include "wmv_engine.h"
#include "model_exporter_header.h"
#include <QSharedPointer>
#include <qstring.h>
#include <qlist.h>

namespace WMVEngine {

// This increments every time we change the ModelExporterServer or ModelExporterType classes
#define MODELEXPORTERSERVERVERSION 1;

// Dummy classes
class ModelExporter {};
class M2File {};
class WMOFile {};
class ADTFile {};

class ModelExporterServer {
public:

	// Exporter
	class ModelExporterType {
	public:
		// Destructor
		virtual ~ModelExporterType() {};
		
		// Get the Plugin Name, as listed in the About Plugins Window
		virtual const QString &getName() const = 0;
		
		// Get the basic information about this exporter
		virtual const ExporterInfo &getExporterInfo() const = 0;

		// Get the capabilities of this exporter
		virtual const ExporterCanDo &exporterCapabilities() const = 0;
		
		// Exporter Functions
		virtual QSharedPointer<ModelExporter> exportM2File(M2File *m2) = 0;			// Export an M2 file
		virtual QSharedPointer<ModelExporter> exportWMOFile(WMOFile *wmo) = 0;		// Export an WMO file
		virtual QSharedPointer<ModelExporter> exportADTFile(ADTFile *adt) = 0;		// Export an ADT file
	};

	// Destructor
	~ModelExporterServer() {
		m_ModelExporterTypes.clear();
	}

	// Allows plugins to add new Exporters
	void addExporter(QSharedPointer<ModelExporterType> ET){
		ExporterInfo info = ET.data()->getExporterInfo();
		#if defined(_WINDOWS)
			if (info.OS_Windows > -1){
				m_ModelExporterTypes.push_back(ET.data());
				return;
			}
		#elif defined(_MAC)
			if (info.OS_MacOSX > -1){
				m_ModelExporterTypes.push_back(ET.data());
				return;
			}
		#elif defined(_LINUX)
			if (info.OS_Linux > -1){
				m_ModelExporterTypes.push_back(ET.data());
				return;
			}
		#endif
	}

	// Get the total number of registered Exporters
	size_t getExporterTypeCount() const {
		return m_ModelExporterTypes.size();
	}

	// Access an Exporter by it's Index
	ModelExporterType &getExporter(size_t Index){
		return *m_ModelExporterTypes.value((int)Index);
	}

private:
	// Vector List of installed Exporters
	typedef QList<ModelExporterType *> ModelExporterTypeVector;

	ModelExporterTypeVector m_ModelExporterTypes;		// All of the Exporters!
};

}	// End Namespace

#endif