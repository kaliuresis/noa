//TODO: check how easy easyzoom integration would be, mouse hover coordinates, click to set position, orb tooltips on map, way to mark found orbs (include separate marking for parallels)

var map_canvas = null;
var ctx = null;
var width = null
var height = null
var scale = 0

Random = Module.cwrap('Random', 'number', ['number', 'number'])
SetRandomSeed = Module.cwrap('SetRandomSeed', null, ['number', 'number', 'number'])
search_spiral_start = Module.cwrap('search_spiral_start', 'number', ['number', 'number', 'number', 'number', 'number'])
search_spiral_step = Module.cwrap('search_spiral_step', 'number', ['number'])
search_portal_start = Module.cwrap('search_portal_start', 'number', ['number', 'number', 'number'])
search_portal_step = Module.cwrap('search_portal_step', 'number', ['number'])

function refresh_canvas_size()
{
    width = map_canvas.width
    height = map_canvas.height
    scale = height/48
}

function init()
{
    var inputs = document.getElementsByTagName("input");
    for(i = 0; i < inputs.length; i++)
    {
        if(inputs[i].getAttribute("type") == "number")
        {
            input = inputs[i]
            increment_box = document.createElement('div');
            increment_box.className = "increment_box";
            inc_button = document.createElement('button');
            dec_button = document.createElement('button');
            increment_box.appendChild(inc_button);
            increment_box.appendChild(dec_button);
            input.insertAdjacentElement("afterend", increment_box);

            inc_button.innerHTML = "▲";
            dec_button.innerHTML = "▼";
            inc_button.type = "button"
            dec_button.type = "button"
            inc_button.tabIndex = -1
            dec_button.tabIndex = -1

            function make_increment_function(input, amount)
            {
                return function()
                {
                    input.value = parseInt(input.value)+amount;
                }
            }

            inc_button.onclick = make_increment_function(input, +1);
            dec_button.onclick = make_increment_function(input, -1);
        }
    }

    map_canvas = document.getElementById('map_canvas');
    refresh_canvas_size()
    ctx = map_canvas.getContext('2d');
}

function reset_xy()
{
    var x_input = document.getElementById("x");
    var y_input = document.getElementById("y");
    x_input.value = 0;
    y_input.value = 0;
}

function far_xy()
{
    var x_input = document.getElementById("x");
    var y_input = document.getElementById("y");
    var big_number = 1050000;
    var x_sign = -(y_input.value > 32*512 ? 1:-1);
    var y_sign = (x_input.value > 28*512 ? 1:-1);
    x_input.value = x_sign*big_number;
    y_input.value = y_sign*big_number;
}

var parallel_width = (64*512)
var parallel_left_edge_tile = -32
var parallel_left_edge = parallel_left_edge_tile*512

function world_to_canvas(x, y)
{
    const world_width = 64*512;
    const world_height = world_width*48/64;
    return [width*x/world_width-parallel_left_edge_tile*scale,
            height*y/world_height+14*scale]
}

var world_seed = 0;
var effective_world_seed = 0;
var ng = 0;
var x0 = 0;
var y0 = 0;
var find_sampo = false;
var search_spiral = false;
var search_portal = false;
var animation_request_id;
var mcguffin_name = "Great Chest Orb"

function paint_cave( x, y, dir, length )
{
	for(var i = 1; i < length+1; i++){

		// BiomeMapSetPixel( x, y, biome_color )

		if( i < 5 || Random( 0, 100 ) < 75 ) {
			x = x + dir
        }
		else
        {
			x = x - dir
		}

		if( x < 2 ) { x = 2 }
		if( x > 62 ) { x = 62 }

		// BiomeMapSetPixel( x, y, biome_color )

		if( i > 3 ) {
			if( Random( 0, 100 ) < 65 ) {
				y = y + 1
            } else {
                y = y - 1
			}
		}

		if( y < 17 ) { y = 17 }
		if( y > 45 ) { y = 45 }

		if( i > 6 ) {
			if( Random( 0, 100 ) < 35 ) {}
			if( Random( 0, 100 ) < 35 ) {}
			if( Random( 0, 100 ) < 35 ) {}
			if( Random( 0, 100 ) < 35 ) {}
		}
    }
}

function paint_biome_area_split( x, y, w, h, buffer )
{
	var extra_width = Random( 0, buffer )
	x = x - extra_width
	w = w + extra_width + Random( 0, buffer )

	var cutoff_point = Random( y + 1, y+h - 2 )

	for(var ix=x; ix < x+w; ix++)
    {
		cutoff_point = cutoff_point + Random( -1, 1 )
		// cutoff_point = clamp( cutoff_point, y + 1, y+h-2 )
    }
}

var orb_list = [[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],]
var orb_indices = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]

function find_normal_orbs()
{
    SetRandomSeed(effective_world_seed, 4573, 4621 )

    //Call Random once for each time it is used by other generation stuff
    var n_random_calls = 0

    if(ng % 3 == 0)
    {
        var n_biomes = 13
        n_random_calls +=  n_biomes-1
    }

    var do_walls = (ng % 5 == 0)

    //rolls to swap biomes
    n_random_calls += 6;

    for(var i = 0; i < n_random_calls; i++)
    {
        Random(0, 0);
    }
    n_random_calls = 0;


    if( Random( 0, 100 ) < 65 ) { paint_cave( 27, 15, -1, Random( 4, 50 ) ) }
    if( Random( 0, 100 ) < 65 ) { paint_cave( 35, 15, 1, Random( 4, 50 ) ) }

    if( Random( 0, 100 ) < 65 ) { paint_cave( 27, 18, -1, Random( 4, 50 ) ) }
    if( Random( 0, 100 ) < 65 ) { paint_cave( 35, 18, 1, Random( 4, 50 ) ) }

    if( Random( 0, 100 ) < 65 ) { paint_cave( 27, 20 + Random( 0, 5 ), -1, Random( 5, 50 ) ) }
    if( Random( 0, 100 ) < 65 ) { paint_cave( 35, 20 + Random( 0, 5 ), 1, Random( 5, 50 ) ) }

    if( Random( 0, 100 ) < 65 ) { paint_cave( 27, 27 + Random( 0, 6 ), -1, Random( 5, 50 ) ) }
    if( Random( 0, 100 ) < 65 ) { paint_cave( 35, 27 + Random( 0, 6 ), 1, Random( 5, 50 ) ) }

    n_random_calls += 2*3;
    for(var i = 0; i < n_random_calls; i++)
    {
        Random(0, 0);
    }
    n_random_calls = 0

    paint_biome_area_split( 28, 20, 7, 6, 3 )

    paint_biome_area_split( 28, 27, 7, 4, 4 )
    paint_biome_area_split( 28, 29, 7, 5, 4 )

    n_random_calls += 2;

    if(do_walls)
    {
        n_random_calls += 2+2*2;
    }
    for(var i = 0; i < n_random_calls; i++)
    {
        Random(0, 0);
    }
    n_random_calls = 0

    //Ready to generate orb locations
    var x;
    var y;

    // pyramid at 51,11
    orb_list[0] = [51,11]

    // floating island
    orb_list[1] = [33,11]

    // console.log("Random(0, 0x1000000)'s")

    // console.log(Random(0, 0x1000000))
    // console.log(Random(0, 0x1000000))

    // console.log(Random(0, 0x1000000))
    // console.log(Random(0, 0x1000000))

    // vault 2
    x = Random( 0, 5 ) + 10
    y = Random( 0, 2 ) + 18
    orb_list[2] = [x,y]

    // inside pyramid
    x = Random( 0, 5 ) + 49
    y = Random( 0, 3 ) + 17
    orb_list[3] = [x,y]

    // hell
    x = Random( 0, 9 ) + 27
    y = Random( 0, 2 ) + 44
    if( ng == 3 || ng >= 25 )
    {
        y = 47
    }
    orb_list[4] = [x, y]

    // snowcave bottom
    x = Random( 0, 6 ) + 12
    y = Random( 0, 3 ) + 40
    orb_list[5] = [x,y]

    // desert bottom
    x = Random( 0, 4 ) + 51
    y = Random( 0, 5 ) + 41
    orb_list[6] = [x,y]


    // TODO (nuke) //NOTE(kaliuresis): this todo was in the original lua code
    x = Random( 0, 5 ) + 58
    y = Random( 0, 5 ) + 34
    orb_list[7] = [x,y]

    x = Random( 0, 9 ) + 40
    y = Random( 0, 11 ) + 21
    orb_list[8] = [x,y]

    x = Random( 0, 7 ) + 17
    y = Random( 0, 8 ) + 21
    orb_list[9] = [x,y]

    x = Random( 0, 7 ) + 1
    y = Random( 0, 9 ) + 24
    orb_list[10] = [x,y]

    for(var i = 0; i < orb_list.length; i++)
    {
        var x2 = orb_list[i][0] - 32;
        var y2 = orb_list[i][1] - 14;

        orb_list[i][0] = x2;
        orb_list[i][1] = y2;
    }
}

function draw_disc(x, y, r)
{
    ctx.beginPath();
    ctx.arc(x, y, r, 0, 2 * Math.PI);
    ctx.fill();
    ctx.stroke();
}

function draw_line_segment(x0, y0, x1, y1)
{
    ctx.beginPath();
    ctx.moveTo(x0, y0);
    ctx.lineTo(x1, y1);
    ctx.stroke();
}

function get_parallel_name(parallel_number, show_main_world = true)
{
    return ((parallel_number > 0) ? "East "+parallel_number
            : ((parallel_number < 0) ? "West "+(-parallel_number)
               : (show_main_world) ? "Main World" : ""));
}


var search_x = 0;
var search_y = 0;
var searched_pixels = 0;
var search_parallel_number = 0;
var search_portal_number = 0;
var search_portal_newgame = 0;
var search_color = "#FFFFFF";
var search_color2 = "#FFFFFF";
var show_search = true;
var map_title;

function redraw_map()
{
    // Draw the orbs on the map
    ctx.clearRect(0, 0, width, height);
    for(var i = 0; i < orb_list.length; i++)
    {
        var x = orb_list[i][0];
        var y = orb_list[i][1];
        ctx.lineWidth = 2
        ctx.fillStyle = "#3892E5";
        ctx.strokeStyle = "#FFFFFF";
        draw_disc((x-parallel_left_edge_tile+0.5)*scale, (y+14+0.5)*scale, 0.5*scale)
    }

    if(show_search)
    {
        var show_x = ((((search_x-parallel_left_edge)%parallel_width)+parallel_width)%parallel_width)+parallel_left_edge;
        var parallel_number = Math.floor((search_x-parallel_left_edge)/parallel_width)
        map_title.innerHTML = get_parallel_name(parallel_number, false)

        p = world_to_canvas(show_x, search_y);
        var x = p[0]
        var y = p[1]

        ctx.fillStyle = search_color;
        ctx.strokeStyle = search_color2;

        ctx.lineWidth = 5
        var a = 0.5*scale
        draw_line_segment(x-a, y-a, x+a, y+a);
        draw_line_segment(x-a, y+a, x+a, y-a);

        ctx.lineWidth = 2
        draw_disc(x, y, 0.3*scale);
    }
}

function update_orbs()
{
    var output = document.getElementById("output");
    output.innerHTML = "";
    var status = document.getElementById("status");
    status.innerHTML = "Searching...";
    map_title = document.getElementById("map_title")
    map_title.innerHTML = "";

    var seed_input = document.getElementById("seed");
    var ng_input = document.getElementById("ng");
    var x_input = document.getElementById("x");
    var y_input = document.getElementById("y");
    var find_sampo_input = document.getElementById("find_sampo")
    var search_spiral_input = document.getElementById("search_spiral")
    // var search_portal_input = document.getElementById("search_portal")
    var newgame_plus_map = document.getElementById("newgame_plus_map")
    var newgame_map = document.getElementById("newgame_map")
    world_seed = parseInt(seed.value)
    ng = parseInt(ng_input.value)
    effective_world_seed = world_seed+ng
    x0 = parseFloat(x_input.value)
    y0 = parseFloat(y_input.value)
    find_sampo = find_sampo_input.checked
    search_spiral = search_spiral_input.checked
    // search_portal = search_portal_input.checked
    mcguffin_name = (find_sampo ? "Sampo" : "Great Chest Orb")

    if(ng > 0)
    {
        map_canvas.width = 1280
        refresh_canvas_size()
        newgame_map.style.display = "none"
        newgame_plus_map.style.display = "block"
    }
    else
    {
        map_canvas.width = 1280*70/64
        refresh_canvas_size()
        newgame_map.style.display = "block"
        newgame_plus_map.style.display = "none"
    }

    parallel_width = (ng > 0) ? (64*512) : (70*512)
    parallel_left_edge_tile = (ng > 0) ? (-32) : (-35)
    parallel_left_edge = parallel_left_edge_tile*512
    var find_chest_orb = search_spiral || search_portal

    orb_list = [
        [1,-3],
	    [19,-3],
        [-20,5],
	    [6,3],
	    [19,5],
        [-9,7],
        [-8,19],
        [8,1],
        [-1,31],
        [-18,28],
        [20,31],
    ];
    orb_indices = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]

    if(ng > 0)
    {
        // orb_ids = [1, 0, 2, 4, 8, 9, 10, 3, 5, 6, 7]
        orb_indices = [1, 0, 2, 7, 3, 8, 9, 10, 4, 5, 6]
        find_normal_orbs();
    }

    for(var i = 0; i < orb_list.length; i++)
    {
        var x = orb_list[orb_indices[i]][0];
        var y = orb_list[orb_indices[i]][1];

        output.innerHTML += "<p>Orb " + i + " at x = " + (x+0.5)*512 + ", y = " + (y+0.5)*512 + "<\p>";
    }

    show_search = search_spiral;
    redraw_map();

    search_x = x0
    search_y = y0
    searched_pixels = 0

    var parallel_name = ""
    //TODO: stop this loop if button is pressed a second time, currently prints twice and wastes compute
    function search_step(timestamp)
    {
        search_color = "#7F5476"
        search_color2 = "#7F5476"
        search_x = getValue(search_spiral_result_ptr, "double");
        search_y = getValue(search_spiral_result_ptr+8, "double");
        searched_pixels = getValue(search_spiral_result_ptr+16, "double");
        redraw_map();
        status.innerHTML = search_iters + " pixels checked...";

        var found_spiral = false
        found_spiral = search_spiral_step(24837)

        if(!found_spiral)
        {
            animation_request_id = window.requestAnimationFrame(search_step);
            return;
        }

        search_x = getValue(search_spiral_result_ptr, "double");
        search_y = getValue(search_spiral_result_ptr+8, "double");

        ret_string_x = search_x.toString()
        ret_string_y = search_y.toString()

        //make ranges for rounding imprecision
        //this continues to be the worst way to implement these things
        if(Math.abs(search_x) > 1000000) {
            var plusminus = 5
            if(Math.abs(search_x) > 10000000) plusminus = 50
            ret_string_x = (search_x).toString() + " \u00B1 " + (plusminus).toString()
        }

        if(Math.abs(search_y) > 1000000) {
            var plusminus = 5
            if(Math.abs(search_y) > 10000000) plusminus = 50
            ret_string_y = (search_y).toString() + " \u00B1 " + (plusminus).toString()
        }

        search_color = "#FF5E26"
        search_color2 = "#FFE385"
        redraw_map();
        output.innerHTML += "<p>"+mcguffin_name+" found at</p><p>x = " + ret_string_x + ", y = " + ret_string_y + "<\p>";
        status.innerHTML = "";
    }

    var found_portal = false
    var search_portal_x = 0
    var search_portal_y = 0
    if(search_portal)
    {
        var search_portal_result_ptr = search_portal_start(world_seed, 0, find_sampo);

        var found_portal = search_portal_step(300)

        search_parallel_number = getValue(search_portal_result_ptr, "i64");
        search_portal_number   = getValue(search_portal_result_ptr+8, "i64");
        search_portal_newgame  = getValue(search_portal_result_ptr+16, "i64");
        search_portal_x = getValue(search_portal_result_ptr+24, "double");
        search_portal_y = getValue(search_portal_result_ptr+32, "double");

        parallel_name = get_parallel_name(search_parallel_number)
        // output.innerHTML += "Searching for portals in "+parallel_name+"...";

        if(found_portal)
        {
            redraw_map();
            var portal_name = ["top left",
                               "top right",
                               "middle left",
                               "middle right",
                               "bottom left",
                               "bottom right",]

            output.innerHTML += "<p>"+mcguffin_name+" found in NG+"+search_portal_newgame+" at destination of " + parallel_name + " " + portal_name[search_portal_number] +
                " portal room portal,</p><p>x = " + search_portal_x + ", y = " + search_portal_y + "<\p>";
        }
        else
        {
            output.innerHTML += "<p>No "+mcguffin_name+"s found in portal room destinations up to NG+"+(search_portal_newgame-1)+", parallels < "+Math.abs(search_parallel_number);
        }
    }

    //start the search
    var search_spiral_result_ptr = search_spiral_start(world_seed, ng, x0, y0, find_sampo);
    window.cancelAnimationFrame(animation_request_id);
    if(search_spiral) animation_request_id = window.requestAnimationFrame(search_step);
    else status.innerHTML = "";

    return false;
}
