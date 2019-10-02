-- _PC_SHALLOW_COPY
function table._PC_SHALLOW_COPY(t)
    local t2 = {}
    for k,v in pairs(t) do
        t2[k] = v
    end
    return t2
end
-- END _PC_SHALLOW_COPY

-- _PC_NEW_MULTI_DIM_ARRAY
function table._PC_NEW_MULTI_DIM_ARRAY(dim)
    local MT = {};
    for i=0, dim do
        MT[i] = {__index = function(t, k)
            if i < dim then
                t[k] = setmetatable({}, MT[i+1])
                return t[k];
            end
        end}
    end

    return setmetatable({}, MT[0]);
end
-- _PC_NEW_MULTI_DIM_ARRAY