#!/usr/bin/groovy
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * */

import javax.imageio.ImageIO

if(args.length == 0) {
	println "Usage: tooled.groovy <image>"
	System.exit(-1)
}

args.each { fn ->
	image = ImageIO.read(new File(fn))

	println "{"
	
	(image.height / 8).times() { y ->
		print "\t"
		image.width.times { x ->
			int i = 0
			(7..0).each {
				i = (i << 1) | (image.getRGB(x, (y * 8) + it) == -1 ? 0 : 1)
			}
			print "0x${Integer.toString(i, 16).with { it.length() < 2 ? '0' + it : it }}, "
		}
		println ""
	}

	println "},"
}
