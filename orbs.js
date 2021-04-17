//TODO: change map based on ng vs ng+, show parallels(and hell?), check how easy easyzoom integration would be, mouse hover coordinates, click to set position, orb tooltips on map

var map_canvas = null;
var ctx = null;
var width = null
var height = null
var scale = 0

Random = Module.cwrap('Random', 'number', ['number', 'number'])
SetRandomSeed = Module.cwrap('SetRandomSeed', null, ['number', 'number', 'number'])
search_spiral_start = Module.cwrap('search_spiral_start', 'number', ['number', 'number', 'number', 'number'])
search_spiral_step = Module.cwrap('search_spiral_step', 'number', ['number'])

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
    ctx = map_canvas.getContext('2d');
    width = map_canvas.width
    height = map_canvas.height
    scale = width/64
}

function reset_xy()
{
    var x_input = document.getElementById("x");
    var y_input = document.getElementById("y");
    x_input.value = 0;
    y_input.value = 0;
}

function world_to_canvas(x, y)
{
    const world_width = 64*512;
    const world_height = world_width*48/64;
    return [width*x/world_width+32*scale,
            height*y/world_height+14*scale]
}

var world_seed = 0;
var effective_world_seed = 0;
var ng = 0;
var x0 = 0;
var y0 = 0;

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

orb_list = [[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],]
orb_ids = [1, 0, 2, 4, 8, 9, 10, 3, 5, 6, 7]

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
        n_random_calls += 2+2*3;
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

var search_x = 0;
var search_y = 0;
var search_color = "#FFFFFF";
var search_color2 = "#FFFFFF";
var show_search = true;

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
        draw_disc((x+32+0.5)*scale, (y+14+0.5)*scale, 0.5*scale)
    }

    if(show_search)
    {
        p = world_to_canvas(search_x, search_y);
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

function update_orbs(find_chest_orb=true)
{
    output = document.getElementById("output");
    output.innerHTML = "";

    var seed_input = document.getElementById("seed");
    var ng_input = document.getElementById("ng");
    var x_input = document.getElementById("x");
    var y_input = document.getElementById("y");
    world_seed = parseInt(seed.value)
    ng = parseInt(ng_input.value)
    effective_world_seed = world_seed+ng
    x0 = parseFloat(x_input.value)
    y0 = parseFloat(y_input.value)

    orb_list = [
        [8,1],
        [1,-3],
        [-9,7],
        [-8,19],
        [-18,28],
        [-20,5],
        [-1,31],
        [20,31],
	    [19,5],
	    [6,3],
	    [19,-3],
    ];

    if(ng > 0)
    {
        find_normal_orbs();
    }

    for(var i = 0; i < orb_list.length; i++)
    {
        var x = orb_list[i][0];
        var y = orb_list[i][1];

        output.innerHTML += "<p>Orb " + orb_ids[i] + " at x = " + (x+0.5)*512 + ", y = " + (y+0.5)*512 + "<\p>";
    }

    show_search = find_chest_orb;
    if(!find_chest_orb)
    {
        redraw_map();
        return false;
    }

    search_x = x0
    search_y = y0

    //TODO: stop this loop if button is pressed a second time, currently prints twice and wastes compute
    function search_step(timestamp)
    {
        search_color = "#7F5476"
        search_color2 = "#7F5476"
        search_x = getValue(search_result_ptr, "double");
        search_y = getValue(search_result_ptr+8, "double");
        redraw_map();

        if(!search_spiral_step(20000))
        {
            window.requestAnimationFrame(search_step);
            return;
        }

        search_x = getValue(search_result_ptr, "double");
        search_y = getValue(search_result_ptr+8, "double");

        search_color = "#FF5E26"
        search_color2 = "#FFE385"
        redraw_map();
        output.innerHTML += "<p>Great Chest Orb found at</p><p>x = " + search_x + ", y = " + search_y + "<\p>";
    }

    //start the search
    var search_result_ptr = search_spiral_start(world_seed, ng, x0, y0);
    window.requestAnimationFrame(search_step);

    return false;
}

function update_orbs_all()
{
    return update_orbs(true);
}

function update_orbs_normal()
{
    return update_orbs(false);
}
