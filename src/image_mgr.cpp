/*
 * Stellarium
 * This file Copyright (C) 2005 Robert Spearman
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

// manage an assortment of script loaded images

#include <iostream>
#include "image_mgr.h"
#include "Projector.hpp"
#include "Navigator.hpp"

ImageMgr::ImageMgr()
{}

ImageMgr::~ImageMgr()
{
    drop_all_images();
}

int ImageMgr::load_image(string filename, string name, Image::IMAGE_POSITIONING position_type)
{

    // if name already exists, replace with new image (hash would have been easier...)
    for (vector<Image*>::iterator iter = active_images.begin(); iter != active_images.end(); ++iter)
    {
        if ((*iter)->get_name()==name)
        {
            delete (*iter);
            active_images.erase(iter);
            break;
        }
    }

    Image *img = new Image(filename, name, position_type);

    if (!img || img->image_loaded())
    {
        active_images.push_back(img);
        return 1;
    }
    else return 0;

}

int ImageMgr::drop_image(string name)
{
    for (vector<Image*>::iterator iter = active_images.begin(); iter != active_images.end(); ++iter)
    {
        if ((*iter)->get_name()==name)
        {
            delete (*iter);
            active_images.erase(iter);
            return 1;
        }
    }
    return 0;  // not found
}

int ImageMgr::drop_all_images()
{

    for (vector<Image*>::iterator iter = active_images.begin(); iter != active_images.end(); ++iter)
    {
        delete *iter;
    }
    active_images.clear();
    return 0;
}

Image * ImageMgr::get_image(string name)
{
    for (vector<Image*>::iterator iter = active_images.begin(); iter != active_images.end(); ++iter)
    {
        if ((*iter)->get_name()==name) return (*iter);
    }
    return NULL;
}

void ImageMgr::update(double delta_time)
{
    for (vector<Image*>::iterator iter = active_images.begin(); iter != active_images.end(); ++iter)
    {
        (*iter)->update(delta_time);
    }
}

double ImageMgr::draw(Projector *prj, const Navigator *nav, ToneReproducer *eye)
{
	prj->set_orthographic_projection();
    for (vector<Image*>::iterator iter = active_images.begin(); iter != active_images.end(); ++iter)
    {
        (*iter)->draw(nav, prj);
    }
    prj->reset_perspective_projection();
	return 0.0;  // TODO: determine max pixel movement from images
}

