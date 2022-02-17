import {ICncharTool} from './tool';

export declare type SpellArg = 'array' | 'low' | 'up' | 'first' | 'poly' | 'tone' | 'simple' | 'trad';
export declare type StrokeArg = 'letter' | 'shape' | 'count' | 'name' | 'detail' | 'array' | 'order' | 'simple' | 'trad';
export declare type SpellToWordArg = 'poly' | 'alltone' | 'array' | 'simple' | 'trad';
export declare type StrokeToWordArg = 'array' | 'simple' | 'trad';
export declare type OrderToWordArg = 'match'  | 'matchorder' | 'contain' | 'start' | 'array' | 'simple' | 'trad';
export declare type IdomArg = 'char' | 'stroke' | 'spell' | 'tone';
export declare type SortSpellArg = 'tone' | 'desc';
export declare type TradArg = 'trad' | 'simple' | 'spark';
export declare type XhyArg = 'fuzzy' | 'answer' | 'second';

export declare type AllArgs = SpellArg | StrokeArg
    | SpellToWordArg | StrokeToWordArg | OrderToWordArg
    | IdomArg | SortSpellArg | TradArg | XhyArg;

export declare type PluginArg = 'order' | 'trad' | 'poly' | 'draw' | 'idiom' | 'xhy' | 'radical';
export declare type ToneType = 0 | 1 | 2 | 3 | 4;
export declare type CompareType = 'more' | 'less' | 'even' | 'error';


export declare interface ISpellInfoReturn {
    spell: string;
    initial: string;
    final: string;
    tone: number;
    index: number;
}

export declare type TypeProp = 'spell' | 'stroke' | 'spellToWord' | 'strokeToWord' | 'orderToWord' | 'idiom' | 'xhy' | 'radical';

export declare type TypeValueObject = {
    [prop in AllArgs]?: AllArgs;
};

export declare interface ISpell {(sentence: string, ...args: Array<SpellArg>): string | Array<any>;}

declare interface IStroke {(sentence: string, ...args: Array<StrokeArg>): number | Array<any>;}

export interface ICnChar {
    spell: ISpell;
    stroke: IStroke;
    use(...plugins: Array<Function>): void;
    spellToWord(spell: string, ...args: Array<SpellToWordArg>): string | Array<string>;
    strokeToWord(stroke: number, ...args: Array<StrokeToWordArg>): string | Array<string>;
    spellInfo: {
        (spell: string): ISpellInfoReturn;
        tones: Array<string>;
        initials: Array<string>;
    };
    plugins: Array<PluginArg>;
    type: {
        [prop in TypeProp]?: TypeValueObject;
    };
    check: boolean;
    readonly version: string;

    transformTone(spell: string, tone?: boolean, type?: 'low' | 'up'): {
        spell: string;
        tone: ToneType;
        index: number;
        isTrans: boolean;
    };
    isCnChar(word: string): boolean;
    isPolyWord(word: string): boolean;
    compareSpell(spell1: string, spell2: string, tone?: boolean): CompareType;
    compareStroke(stroke1: string | number, stroke2: string | number): CompareType;
    sortSpell(spells:Array<string> | string, ...args: Array<SortSpellArg>): Array<string> | string;
    sortStroke(strokes:Array<string|number> | string, desc?: 'desc'): Array<string | number> | string;
    setSpellDefault(word: string, spell: string): void;
    setSpellDefault(json: {[key: string]: string}): void;
    setSpell(word: string, spells: string | Array<string>): void;
    setSpell(json: {[key: string]: string | Array<string>}): void;
    setStrokeCount(word: string, count: number): void;
    setStrokeCount(json: {[key: string]: number}): void;
    shapeSpell(spell: string): string;

    _: ICncharTool;
    _origin: {
        spell: ISpell;
        stroke: IStroke;
    };
}

declare global {
    interface String {
        spell(...args: Array<SpellArg>): string | Array<any>;
        stroke(...args: Array<StrokeArg>): number | Array<any>;
    }
}
