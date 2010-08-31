#include "dbcfile.h"
#include "mpq.h"

DBCFile::DBCFile(const wxString &filename) : filename(filename)
{
	data = NULL;
}

bool DBCFile::open()
{
	MPQFile f((char *)filename.c_str());
	int db_type = 0;

	// Need some error checking, otherwise an unhandled exception error occurs
	// if people screw with the data path.
	if (f.isEof()) {
		if (filename.AfterLast('.') == _T("dbc")) {
			wxString fn = filename.BeforeLast('.') + _T(".db2");
			f.openFile(fn);
			if (f.isEof())
				return false;
		} else
			return false;
	}

	char header[5];
	unsigned int na,nb,es,ss;

	f.read(header, 4); // File Header

	if (strncmp(header, "WDBC", 4) == 0)
		db_type = 1;
	else if (strncmp(header, "WDB2", 4) == 0)
		db_type = 2;

	if (db_type == 0) {
		f.close();
		data = NULL;
		wxLogMessage(_T("Critical Error: An error occured while trying to read the DBCFile %s."), filename.c_str());
		return false;
	}

	//assert(header[0]=='W' && header[1]=='D' && header[2]=='B' && header[3] == 'C');

	f.read(&na,4); // Number of records
	f.read(&nb,4); // Number of fields
	f.read(&es,4); // Size of a record
	f.read(&ss,4); // String size

	if (db_type == 2)
		f.seekRelative(28);
	
	recordSize = es;
	recordCount = na;
	fieldCount = nb;
	stringSize = ss;
	//assert(fieldCount*4 == recordSize);
	// not always true, but it works fine till now
	assert(fieldCount*4 >= recordSize);

	data = new unsigned char[recordSize*recordCount+stringSize];
	stringTable = data + recordSize*recordCount;
	f.read(data, recordSize*recordCount+stringSize);
	f.close();
	return true;
}

DBCFile::~DBCFile()
{
	delete [] data;
}

DBCFile::Record DBCFile::getRecord(size_t id)
{
	assert(data);
	return Record(*this, data + id*recordSize);
}

DBCFile::Iterator DBCFile::begin()
{
	assert(data);
	return Iterator(*this, data);
}
DBCFile::Iterator DBCFile::end()
{
	assert(data);
	return Iterator(*this, stringTable);
}

