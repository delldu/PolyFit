/*
Copyright (C) 2017  Liangliang Nan
https://3d.bk.tudelft.nl/liangliang/ - liangliang.nan@gmail.com

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "point_set_io.h"
#include "point_set_serializer_vg.h"
#include "../model/point_set.h"
#include "../basic/stop_watch.h"
#include "../basic/file_utils.h"
#include "../basic/logger.h"

#include <fstream>
#include <list>


PointSet* PointSetIO::read(const std::string& file_name)
{
	std::ifstream in(file_name.c_str()) ;
	if(in.fail()) {
		Logger::err("-") << "cannot open file: " << file_name << std::endl;
		return nil;
	}
	in.close();

	Logger::out("-") << "reading file..." << std::endl;

	std::string ext = FileUtils::extension(file_name);
	String::to_lowercase(ext);

	StopWatch w;
	PointSet* pset = new PointSet;
	if (ext == "vg")
		PointSetSerializer_vg::load_vg(pset, file_name);
	else if (ext == "bvg")
		PointSetSerializer_vg::load_bvg(pset, file_name);

	else {
		Logger::err("-") << "reading file failed (unknown file format)" << std::endl;
		delete pset;
		return nil;
	}
		
	if (pset->num_points() < 1) {
		Logger::err("-") << "reading file failed (no data exist)" << std::endl;
		delete pset;
		return nil;
	}

	Logger::out("-") << "done. " << w.elapsed() << " sec." << std::endl;

	return pset;
}

bool PointSetIO::save(const std::string& file_name, const PointSet* point_set) {
	if (!point_set) {
		Logger::err("-") << "Point set is null" << std::endl;
		return false;
	}
	
	std::ofstream out(file_name.c_str()) ;
	if(out.fail()) {
		Logger::err("-") << "cannot open file: \'" << file_name << "\' for writing" << std::endl;
		return false;
	}
	Logger::out("-") << "saving file..." << std::endl;
	out.close();

	StopWatch w;
	std::string ext = FileUtils::extension(file_name);
	String::to_lowercase(ext);
	
	out.precision(16);

 	if (ext == "vg")
		PointSetSerializer_vg::save_vg(point_set, file_name);
	else if (ext == "bvg")
		PointSetSerializer_vg::save_bvg(point_set, file_name);

	else {
		Logger::err("-") << "saving file failed (unknown file format)" << std::endl;
		return false;
	}

	Logger::out("-") << "done. " << w.elapsed() << " sec." << std::endl;
	return true;
}


bool PointSetIO::save_ply(const std::string& file_name, PointSet* point_set) {
	if (!point_set) {
		Logger::err("-") << "Point set is null" << std::endl;
		return false;
	}
	
	std::ofstream out(file_name.c_str()) ;
	if(out.fail()) {
		Logger::err("-") << "cannot open file: \'" << file_name << "\' for writing" << std::endl;
		return false;
	}
	Logger::out("-") << "saving file..." << std::endl;

	StopWatch w;
	std::string ext = FileUtils::extension(file_name);
	String::to_lowercase(ext);

	const std::vector<vec3>& points = point_set->points();

	out.precision(16);


	const std::vector<VertexGroup::Ptr>& groups = point_set->groups();
	std::size_t num_groups = groups.size();

	std::size_t total_points = 0;
	for (std::size_t i = 0; i < num_groups; ++i) {
		VertexGroup* g = groups[i];
		total_points += g->size();
	}

	out <<
	"ply" << std::endl <<
	"format ascii 1.0" << std::endl <<
	"element vertex " << total_points << std::endl <<
	"property float x" << std::endl <<
	"property float y" << std::endl <<
	"property float z" << std::endl <<
	"property uchar red"   << std::endl <<
	"property uchar green" << std::endl <<
	"property uchar blue"  << std::endl <<
	"end_header" << std::endl;


	for (std::size_t i = 0; i < num_groups; ++i) {
		VertexGroup* g = groups[i];
		Color c = g->color();

		std::size_t g_num_point = g->size();
		for (int j = 0; j < g_num_point; j++) {
			int k = g->at(j); // vertex index
			out << points[k][0] << " " << points[k][1] << " " << points[k][2] << " "
				<< (int)(c[0]*255) << " " << (int)(c[1]*255) << " " << (int)(c[2]*255) << std::endl;
		}

		// // children
		// const std::vector<VertexGroup*>& children = g->children();
		// std::size_t chld_num = children.size();
		// for (std::size_t j = 0; j < chld_num; ++j) {
		// 	VertexGroup* chld = children[j];

		// 	std::size_t chld_num_point = chld->size();
		// 	for (int j = 0; j < chld_num_point; j++) {
		// 		int k = chld->at(j); // vertex index
		// 		out << points[k][0] << " " << points[k][1] << " " << points[k][2] << " "
		// 			<< (int)(c[0]*255) << " " << (int)(c[1]*255) << " " << (int)(c[2]*255) << std::endl;
		// 	}
		// }
	}

	out.close();


	Logger::out("-") << "done. " << w.elapsed() << " sec." << std::endl;
	return true;
}
