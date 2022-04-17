import { genInput } from './handle/gen';

export function mock(...args: any) {
    let rurl;
    let rtype = 'get';
    let template;
    switch (args.length) {
        case 1:
            // Mock.mock(template)
            [template] = args;

            const data = genInput(template);
            //debugger 专用
            console.log(template, data);
            return data;

        // 2 和 3 switch 穿透
        case 2:
            // Mock.mock(rurl, template)
            [rurl, template] = args;
            break;
        case 3:
            // Mock.mock(rurl,rtype, template)
            [rurl, rtype, template] = args;
            break;
    }

    return null;
}
