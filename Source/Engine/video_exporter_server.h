/*
 Video Exporter Server

 Exporting is now done with plugin files. This means anyone can write and
 distribute an exporter without needing to upload the code to WMV's code.
*/

#ifndef WMVENGINE_VIDEO_EXPORTER_SERVER_H
#define WMVENGINE_VIDEO_EXPORTER_SERVER_H

#include "wmv_engine.h"
#include <QSharedPointer>
#include <qstring.h>
#include <qlist.h>

namespace WMVEngine {

// This increments every time we change the VideoExporterServer or VideoExporterType classes
#define VIDEOEXPORTERSERVERVERSION 1

// Dummy classes
class VideoExporter {};

class VideoExporterServer {
public:

	// Exporter
	class VideoExporterType {
	public:
		// Destructor
		virtual ~VideoExporterType() {};
		// Get Name
		virtual const QString &getName() const = 0;
		
		// Exporter Functions
		virtual QSharedPointer<VideoExporter> exportVideoFile() = 0;
	};

	// Destructor
	~VideoExporterServer() {
		m_VideoExporterTypes.clear();
	}

	// Allows plugins to add new Exporters
	void addExporter(QSharedPointer<VideoExporterType> ET){
		m_VideoExporterTypes.push_back(ET.data());
	}

	// Get the total number of registered Exporters
	size_t getExporterTypeCount() const {
		return m_VideoExporterTypes.size();
	}

	// Access an Exporter by it's Index
	VideoExporterType &getExporter(size_t Index){
		return *m_VideoExporterTypes.value((int)Index);
	}

private:
	// Vector List of installed Exporters
	typedef QList<VideoExporterType *> VideoExporterTypeVector;

	VideoExporterTypeVector m_VideoExporterTypes;		// All of the Exporters!
};

}	// End Namespace

#endif