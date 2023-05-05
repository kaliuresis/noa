//TODO: check how easy easyzoom integration would be, mouse hover coordinates, click to set position, orb tooltips on map, way to mark found orbs (include separate marking for parallels)

var map_canvas = null;
var ctx = null;
var width = null
var height = null
var scale = 0

search_spiral_start = Module.cwrap('search_spiral_start', 'number', ['number', 'number', 'number', 'number', 'number'])
search_spiral_step = Module.cwrap('search_spiral_step', 'number', ['number'])

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
var capacity = 27;
var animation_request_id;

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
    var capacity_input = document.getElementById("capacity");
    var newgame_plus_map = document.getElementById("newgame_plus_map")
    var newgame_map = document.getElementById("newgame_map")
    world_seed = parseInt(seed.value)
    ng = parseInt(ng_input.value)
    effective_world_seed = world_seed+ng
    x0 = parseFloat(x_input.value)
    y0 = parseFloat(y_input.value)
    capacity = parseInt(capacity_input.value)

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
    var find_chest_orb = true

    show_search = true;
    redraw_map();

    search_x = x0
    search_y = y0

    var parallel_name = ""
    //TODO: stop this loop if button is pressed a second time, currently prints twice and wastes compute
    function search_step(timestamp)
    {
        search_color = "#7F5476"
        search_color2 = "#7F5476"
        search_x = getValue(search_spiral_result_ptr, "double");
        search_y = getValue(search_spiral_result_ptr+8, "double");
        redraw_map();

        var found_spiral = false
        found_spiral = search_spiral_step(19997)

        if(!found_spiral)
        {
            animation_request_id = window.requestAnimationFrame(search_step);
            return;
        }

        search_x = getValue(search_spiral_result_ptr, "double");
        search_y = getValue(search_spiral_result_ptr+8, "double");
        var search_capacity = getValue(search_spiral_result_ptr+16, "double");
        var search_multicast = getValue(search_spiral_result_ptr+24, "double");
        var search_delay = getValue(search_spiral_result_ptr+32, "double");
        var search_reload = getValue(search_spiral_result_ptr+40, "double");
        var search_mana = getValue(search_spiral_result_ptr+48, "double");
        var search_regen = getValue(search_spiral_result_ptr+56, "double");
        var search_spread = getValue(search_spiral_result_ptr+64, "double");
        var search_speead = getValue(search_spiral_result_ptr+72, "double");
        var search_shuffle = getValue(search_spiral_result_ptr+80, "double");

        ret_string_x = search_x.toString()
        ret_string_y = search_y.toString()

        //make ranges for rounding imprecision
        //this continues to be the worst way to implement these things
        if(Math.abs(search_x) > 1000000) {
            var plusminus = 5
            if(Math.abs(search_x) > 10000000) plusminus = 50
            ret_string_x = (search_x - 0.5).toString() + " \u00B1 " + (plusminus - 0.5).toString()
        }

        if(Math.abs(search_y) > 1000000) {
            var plusminus = 5
            if(Math.abs(search_y) > 10000000) plusminus = 50
            ret_string_y = (search_y - 0.5).toString() + " \u00B1 " + (plusminus - 0.5).toString()
        }

        search_color = "#FF5E26"
        search_color2 = "#FFE385"
        redraw_map();
        output.innerHTML += "<p>Wand found at x = " + ret_string_x + ", y = " + ret_string_y + "<\p>";
        output.innerHTML += "<p>Capacity: " + Math.floor(search_capacity) + "<\p>";
        output.innerHTML += "<p>Multicast: " + search_multicast + "<\p>";
        output.innerHTML += "<p>Cast Delay: " + (search_delay / 60).toFixed(2) + " sec<\p>";
        output.innerHTML += "<p>Reload: " + (search_reload / 60).toFixed(2) + " sec<\p>";
        output.innerHTML += "<p>Max Mana: " + search_mana + "<\p>";
        output.innerHTML += "<p>Mana Regen: " + search_regen + "<\p>";
        output.innerHTML += "<p>Spread: " + search_spread + " deg<\p>";
        output.innerHTML += "<p>Speed: " + search_speead.toFixed(3) + "x<\p>";
        output.innerHTML += "<p>" + (search_shuffle == 0 ? "Nonshuffle" : "Shuffle") + "<\p>";
        status.innerHTML = "";
    }

    //start the search
    var search_spiral_result_ptr = search_spiral_start(world_seed, ng, x0, y0, capacity);
    window.cancelAnimationFrame(animation_request_id);
    if(true) animation_request_id = window.requestAnimationFrame(search_step);
    else status.innerHTML = "";

    return false;
}
