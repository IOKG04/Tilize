#! /bin/lua

-- returns true if file_name exists, false otherwise
function file_exists(file_name)
    local f = io.open(file_name, "r")
    if f ~= nil then
        io.close(f)
        return true
    else
        return false
    end
end
-- return hex code portion of hex_code
function color_hex_code(hex_code)
    local start_index, end_index = string.find(hex_code, "^%x%x%x%x%x%x$")
    if start_index == nil or end_index == nil then
        return nil
    end
    return string.sub(hex_code, start_index, end_index)
end
-- return true if table contains element, false otherwise
function contains(table, element)
    for _, value in pairs(table) do
        if value == element then
            return true
        end
    end
    return false
end

-- print help message if requested or required
if arg[1] == nil or arg[1] == "help" or arg[1] == "--help" or arg[1] == "-h" then
    io.write("Usage:\n")
    io.write(" " .. arg[0] .. " [file]  | Create a configuration at [file]\n")
    io.write(" " .. arg[0] .. " help    | Show this help message\n")
    os.exit((arg[1] == nil) and 1 or 0)
end

-- check if output file exists, give warning if so
arg[1] = string.gsub(arg[1], '\\', '/')
if file_exists(arg[1]) then
    io.write(arg[1] .. " already exists. Overwrite (y / N)? ")
    local yN = string.sub(io.read(), 1, 1)
    if not (yN == 'y' or yN == 'Y') then
        io.write("Not overwriting, exiting.\n")
        os.exit(0)
    end
end

-- get pattern path
io.write("Location of pattern? ")
pattern_path_abs = string.gsub(io.read(), '\\', '/')
if not file_exists(pattern_path_abs) then
    io.write(pattern_path_abs .. " does not exist, exiting.\n")
    os.exit(1)
end
pattern_path = string.sub(pattern_path_abs, string.find(pattern_path_abs, "([^/]+)$"))

-- get tile dimensions
io.write("Tile dimensions?\n")
io.write(" Width:  ")
tile_width = math.tointeger(io.read())
io.write(" Height: ")
tile_height = math.tointeger(io.read())
if tile_width == nil or tile_height == nil or tile_width <= 0 or tile_height <= 0 then
    io.write("Tile width and height must be integers above 0, exiting.\n")
    os.exit(1)
end

-- get colors
io.write("Colors in the configuration?\n")
io.write("Input as hex codes (ex. \"FF05A8\"), leave empty to finish.\n")
colors = {}
local col_count = 1
while true do
    io.write(" Color " .. col_count .. "? ")
    local inp = string.lower(io.read())
    if inp == "" then -- break on empty input
        col_count = col_count - 1
        break
    end
    local hex_code = color_hex_code(inp)
    if hex_code == nil then
        io.write("Bad format, input as hex codes (ex. \"FF05A8\"), leave empty to finish.\n")
    elseif contains(colors, hex_code) then
        io.write(hex_code .. " was already inputed.\n")
    else
        colors[col_count] = hex_code
        col_count = col_count + 1
    end
end

-- get background_color
io.write("Background color (leave empty to allow all, otherwise a hex code previously inputed or the number of the color)? ")
::background_color_reinput::
background_color_inp = io.read()
local background_color_hex_code = color_hex_code(background_color_inp)
if background_color_inp == "" then -- allow all
    background_color = -1
elseif background_color_hex_code ~= nil then -- find matching color
    for ind, val in pairs(colors) do
        if val == background_color_hex_code then
            background_color = ind - 1
            break
        end
    end
    if background_color == nil then
        io.write("Provided hex code not in colors previously provided, please input again. ")
        goto background_color_reinput
    end
else -- set to index
    local bci_toint = math.tointeger(background_color_inp)
    if bci_toint == nil or colors[bci_toint] == nil then
        io.write("Provided number has to be between 1 and " .. #colors .. " (inclusive), please input again. ")
        goto background_color_reinput
    end
    background_color = bci_toint - 1
end

-- get foreground_color
io.write("Foreground color (leave empty to allow all, otherwise a hex code previously inputed or the number of the color)? ")
::foreground_color_reinput::
foreground_color_inp = io.read()
local foreground_color_hex_code = color_hex_code(foreground_color_inp)
if foreground_color_inp == "" then -- allow all
    foreground_color = -1
elseif foreground_color_hex_code ~= nil then -- find matching color
    for ind, val in pairs(colors) do
        if val == foreground_color_hex_code then
            foreground_color = ind - 1
            break
        end
    end
    if foreground_color == nil then
        io.write("Provided hex code not in colors previously provided, please input again. ")
        goto foreground_color_reinput
    end
else -- set to index
    local fci_toint = math.tointeger(foreground_color_inp)
    if fci_toint == nil or colors[fci_toint] == nil then
        io.write("Provided number has to be between 1 and " .. #colors .. " (inclusive), please input again. ")
        goto foreground_color_reinput
    end
    foreground_color = fci_toint - 1
end

-- write to file
io.write("Writing configuration to " .. arg[1])
outp_file = io.open(arg[1], "w")
outp_file:write("{\n")
outp_file:write("\t\"pattern_path\": \"" .. pattern_path .. "\",\n")
outp_file:write("\t\"tile_width\": " .. tile_width .. ",\n")
outp_file:write("\t\"tile_height\": " .. tile_height .. ",\n")
outp_file:write("\t\"colors\": [\n")
for ind, val in pairs(colors) do
    outp_file:write("\t\t\"" .. val .. "\"")
    if ind == #colors then
        outp_file:write("\n")
    else
        outp_file:write(",\n")
    end
end
outp_file:write("\t],\n")
outp_file:write("\t\"background_color\": " .. background_color .. ",\n")
outp_file:write("\t\"foreground_color\": " .. foreground_color .. "\n")
outp_file:write("}")
io.close(outp_file)
io.write(" // Done\n")

-- copy pattern_path_abs to pattern_path
local path_start, path_end = string.find(arg[1], "(.*/)")
pattern_path_rel = (string.sub(arg[1], path_start or 0, path_end or 0)) .. pattern_path
io.write("Copying " .. pattern_path_abs .. " to " .. pattern_path_rel)
pp_src = io.open(pattern_path_abs, "r")
pp_dst = io.open(pattern_path_rel, "w")
pp_dst:write(pp_src:read("*a"))
io.close(pp_src)
io.close(pp_dst)
io.write(" // Done\n")
