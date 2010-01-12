/*
 * WoWModelViewer X3D exporter
 * Author: Tobias Alexander Franke
 * Date: 1/12/2010
 *
 */

#include <wx/wfstream.h>
#include <math.h>

#include "modelexport.h"
#include "modelcanvas.h"

#include "CxImage/ximage.h"

class tabbed_ostream
{
private:
    size_t tabc_;
    std::string tab_;
    std::ostream& stream_;
    bool on_;

    void mtab() 
    {
        tab_ = "";
        for (size_t x = 0; x < tabc_; ++x)
            tab_ += "\t";
    };

public:
    tabbed_ostream(std::ostream& stream) : tabc_(0), tab_(""), stream_(stream), on_(true) 
    { 
        stream_.precision(5); 
    }

    void toggle() { on_ = !on_; }
    void tab() { tabc_++; mtab(); }
    void rtab() { tabc_--; mtab(); }

    template<typename T>
    std::ostream& operator << (T c)
    {
        if (on_)
            stream_ << tab_.c_str();
        stream_ << c;
        return stream_;
    }
};

void M2toX3D(tabbed_ostream s, Model *m, bool init, const char* fn, bool exportAnimations, bool xhtml)
{
    s << "<!-- Exported with WoWModelViewer -->" << std::endl;

    s.tab();
    s << "<Scene>" << std::endl;

    s.tab();

    // define background
    s   << "<Background skyColor='" 
        << 70.f/256.f << " " 
        << 94.f/256.f << " "
        << 121.f/256.f << " "
        << "'/>" << std::endl;

    // define all textures
    typedef std::map<int, wxString> texMap;
    texMap textures;

    if (!fn)
        fn = "texture";

    for (size_t i=0; i<m->passes.size(); i++) 
    {
        ModelRenderPass &p = m->passes[i];
        if (p.init(m))
        {
            wxString texName(fn, wxConvUTF8);
            texName = texName.AfterLast('\\').BeforeLast('.');
            texName << _T("_") << p.tex;

            if (!textures[p.tex])
            {
                wxString texFilename(fn, wxConvUTF8);
                texFilename = texFilename.BeforeLast('\\');
                texFilename += '\\';
                texFilename += texName;
                texFilename += ".png";
                SaveTexture(texFilename);

                s << "<ImageTexture DEF='" << texName << "' url='"
                  << texFilename.AfterLast('\\').c_str() << "' />" << std::endl;
            }

            textures[p.tex] = texName;
        }
    }

    for (size_t i=0; i<m->passes.size(); i++) 
    {
        ModelRenderPass &p = m->passes[i];
        if (p.init(m)) 
        {
            s << "<Shape>"  << std::endl;

            size_t counter;

            // write material, color etc.

            s.tab();
            s << "<Appearance>" << std::endl;

            s.tab();
            
            s   << "<Material diffuseColor='"
                << p.ocol.x << " " 
                << p.ocol.y << " " 
                << p.ocol.z << "' ";

            s.toggle();
            if (!xhtml) 
            {
                    
                   s    << "emissiveColor='" 
                        << p.ecol.x << " " 
                        << p.ecol.y << " " 
                        << p.ecol.z << "' "

                        << "transparency='" << p.ocol.w << "' ";
                    
            }
            
            s << "/>" << std::endl;
            s.toggle();
            
            s << "<ImageTexture USE='" << textures[p.tex] << "'/>" << std::endl;

            s.rtab();
            s << "</Appearance>" << std::endl;

            // ---------------- write all indices here -----------------

            s << "<IndexedFaceSet solid='false' ";
            s.toggle();

            // write normals
            counter = 0;
            s << "coordIndex='" << std::endl;
            for (unsigned int k=0; k<p.indexCount; k+=3)
            {
                s << counter+1 << " " << counter+2 << " " << counter+0 << " -1 ";
                counter += 3;
            }
            s << "'>" << std::endl;
            s.toggle();

            // ----------------- write all data here -----------------

            // write vertices
            s.tab();
            s << "<Coordinate DEF='pl_" << i << "' point='" << std::endl;
            s.tab();
            for (size_t k = 0, b=p.indexStart; k<p.indexCount; k++,b++) 
            {
                uint16 a = m->indices[b];
                Vec3D v;

                if (m->vertices == NULL || init == true) 
                    v = m->origVertices[a].pos;
                else
                    v = m->vertices[a];

                s << v.x << " " << v.y << " " << v.z << std::endl;
            }
            s.rtab();
            s << "' />" << std::endl;

            // write normals
            s << "<Normal vector='" << std::endl;
            s.tab();
            for (size_t k = 0, b=p.indexStart; k<p.indexCount; k++,b++) 
            {
                uint16 a = m->indices[b];
                Vec3D v = m->origVertices[a].normal;

                s << v.x << " " << v.y << " " << v.z << std::endl;
            }
            s.rtab();
            s << "' />" << std::endl;

            // write texcoords
            s << "<TextureCoordinate point='" << std::endl;
            s.tab();
            for (size_t k = 0, b=p.indexStart; k<p.indexCount; k++,b++)
            {
                uint16 a = m->indices[b];
                Vec2D v = m->origVertices[a].texcoords;
                s << v.x << " " << 1-v.y << std::endl;
            }
            s.rtab();
            s << "' />" << std::endl;

            s.rtab();
            s << "</IndexedFaceSet>" << std::endl;

            s.rtab();
            s << "</Shape>" << std::endl << std::endl;
        }
    }

#ifdef EXPORTANIMATED
    if (m->animated && exportAnimations)
    {
        size_t animation = m->animManager->GetAnim();
        ModelAnimation* ma = &m->anims[animation];

        size_t animSpeed = ma->playSpeed;

        if (animSpeed == 0) // error
            return;

        size_t numFrames = (ma->timeEnd - ma->timeStart)/animSpeed;

        s   << "<TimeSensor DEF='ts' "
            << "cycleInterval='" << 1.0f+(numFrames/25) << "' "
            << "loop='TRUE' />" << std::endl;
        
        for (size_t i=0; i<m->passes.size(); i++) 
        {
            ModelRenderPass &p = m->passes[i];         

            if (p.init(m)) 
            {

                s << "<CoordinateInterpolator DEF='ci_" << i << "' ";
                s.toggle();
                s << "key='";

                for (size_t frame = 0; frame < numFrames; ++frame)
                    s << static_cast<float>(frame)/static_cast<float>(numFrames) << " ";
                s << " 1.0' keyValue='" << std::endl;
                s.toggle();

                for (size_t frame = 0; frame <= numFrames; ++frame)
                {
                    // set time
                    //m->animManager->Tick(animSpeed*frame);

                    if (frame != numFrames)
                        m->animManager->SetFrame(animSpeed*frame);
                    else
                        m->animManager->SetFrame(0);

                    // calculate frame
                    m->animate(animation);

                    //s << "<!-- frame " << frame << " -->" << std::endl;

                    for (size_t k = 0, b=p.indexStart; k<p.indexCount; k++,b++)
                    {
                        uint16 a = m->indices[b];
                        Vec3D v;

                        v = m->vertices[a];

                        s << v.x << " " << v.y << " " << v.z << " " << std::endl;
                    }

                }
                s << "' />" << std::endl;
                

                s << "<ROUTE fromNode='ts' fromField='fraction_changed' toNode='ci_" << i << "' toField='set_fraction' />" << std::endl;
                s << "<ROUTE fromNode='ci_" << i << "' fromField='value_changed' toNode='pl_" << i << "' toField='set_point' />" << std::endl;
            }
        }
    }
#endif

    s << "</Scene>" << std::endl;
    s.rtab();
}

void ExportM2toX3D(Model *m, const char *fn, bool init, bool animated)
{
    ofstream f(fn, ios_base::out | ios_base::trunc);

    if (f.good())
    {
        f << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
        f << "<!DOCTYPE X3D PUBLIC \"ISO//Web3D//DTD X3D 3.0//EN\" \"http://www.web3d.org/specifications/x3d-3.0.dtd\">" << std::endl;
        f << "<X3D xmlns:xsd='http://www.w3.org/2001/XMLSchema-instance' profile='Full' version='3.0' xsd:noNamespaceSchemaLocation='http://www.web3d.org/specifications/x3d-3.0.xsd'>" << std::endl;
        M2toX3D(f, m, init, fn, animated, false);
        f << "</X3D>" << std::endl;
    }

    f.close();
}

void ExportM2toXHTML(Model *m, const char *fn, bool init)
{
    ofstream f(fn, ios_base::out | ios_base::trunc);

    if (f.good())
    {
        // write xhtml stuff for WebGL
        f << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">" << std::endl;
        f << "<html xmlns=\"http://www.w3.org/1999/xhtml\">" << std::endl;
        f << "<head>" << std::endl;
        f << "    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />" << std::endl;
        f << "    <title>X3DOM example</title>" << std::endl;
        f << "    <link rel=\"stylesheet\" type=\"text/css\" href=\"http://www.x3dom.org/x3dom/src/x3dom.css\" />" << std::endl;
        f << "</head>" << std::endl;
        f << "<body>" << std::endl;
        f << "    <h1>X3DOM example</h1>" << std::endl;
        f << "    <p> " << std::endl;
        f << "    <X3D xmlns=\"http://www.web3d.org/specifications/x3d-namespace\" id=\"someUniqueId\" showStat=\"false\" showLog=\"false\" x=\"0px\" y=\"0px\" width=\"400px\" height=\"400px\">" << std::endl;

        M2toX3D(f, m, init, fn, false, true);

        f << "\n" << std::endl;
        f << "      </X3D>" << std::endl;
        f << "    </p>" << std::endl;
        f << "    <script type=\"text/javascript\" src=\"http://www.x3dom.org/x3dom/src/x3dom.js\"></script>" << std::endl;
        f << "</body>" << std::endl;
        f << "</html>" << std::endl;
    }

    f.close();
}
