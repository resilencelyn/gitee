/**
 * 数据来源
 * https://tailwind.build/classes
 * 执行以下代码
 */

//  const wrapper = document.querySelectorAll('.w-full.mb-6');
//  let json = {

//  }
//  wrapper.forEach(item => {
//      const name = item.querySelector('h5').innerText.replace(' ', '-').toLocaleLowerCase();
//      json[name] = [...item.querySelectorAll('a')].map(item => item.innerText)
//  })
//  console.log(json);

export const useableClassesTransition = [
  "transition",
  "transition-none",
  "transition-all",
  "transition-colors",
  "transition-opacity",
  "transition-shadow",
  "transition-transform",
];

export const useableClassesEase = [
  "ease-linear",
  "ease-in",
  "ease-out",
  "ease-in-out",
];

// 可以使用的classes
export const useableClasses: { [key: string]: string[] } = {
  background: [
    "bg-auto",
    "bg-cover",
    "bg-contain",
    "bg-bottom",
    "bg-top",
    "bg-center",
    "bg-left",
    "bg-left-bottom",
    "bg-left-top",
    "bg-right",
    "bg-right-bottom",
    "bg-right-top",
    "bg-fixed",
    "bg-local",
    "bg-scroll",
    "bg-no-repeat",
    "bg-repeat",
    "bg-repeat-x",
    "bg-repeat-y",
    "bg-repeat-round",
    "bg-repeat-space",
    "bg-none",
  ],

  "background-color": [
    "bg-transparent",
    "bg-current",
    "bg-black",
    "bg-white",
    "bg-gray-50",
    "bg-gray-100",
    "bg-gray-200",
    "bg-gray-300",
    "bg-gray-400",
    "bg-gray-500",
    "bg-gray-600",
    "bg-gray-700",
    "bg-gray-800",
    "bg-gray-900",
    "bg-red-50",
    "bg-red-100",
    "bg-red-200",
    "bg-red-300",
    "bg-red-400",
    "bg-red-500",
    "bg-red-600",
    "bg-red-700",
    "bg-red-800",
    "bg-red-900",
    "bg-orange-50",
    "bg-orange-100",
    "bg-orange-200",
    "bg-orange-300",
    "bg-orange-400",
    "bg-orange-500",
    "bg-orange-600",
    "bg-orange-700",
    "bg-orange-800",
    "bg-orange-900",
    "bg-yellow-50",
    "bg-yellow-100",
    "bg-yellow-200",
    "bg-yellow-300",
    "bg-yellow-400",
    "bg-yellow-500",
    "bg-yellow-600",
    "bg-yellow-700",
    "bg-yellow-800",
    "bg-yellow-900",
    "bg-green-50",
    "bg-green-100",
    "bg-green-200",
    "bg-green-300",
    "bg-green-400",
    "bg-green-500",
    "bg-green-600",
    "bg-green-700",
    "bg-green-800",
    "bg-green-900",
    "bg-teal-50",
    "bg-teal-100",
    "bg-teal-200",
    "bg-teal-300",
    "bg-teal-400",
    "bg-teal-500",
    "bg-teal-600",
    "bg-teal-700",
    "bg-teal-800",
    "bg-teal-900",
    "bg-blue-50",
    "bg-blue-100",
    "bg-blue-200",
    "bg-blue-300",
    "bg-blue-400",
    "bg-blue-500",
    "bg-blue-600",
    "bg-blue-700",
    "bg-blue-800",
    "bg-blue-900",
    "bg-indigo-50",
    "bg-indigo-100",
    "bg-indigo-200",
    "bg-indigo-300",
    "bg-indigo-400",
    "bg-indigo-500",
    "bg-indigo-600",
    "bg-indigo-700",
    "bg-indigo-800",
    "bg-indigo-900",
    "bg-purple-50",
    "bg-purple-100",
    "bg-purple-200",
    "bg-purple-300",
    "bg-purple-400",
    "bg-purple-500",
    "bg-purple-600",
    "bg-purple-700",
    "bg-purple-800",
    "bg-purple-900",
    "bg-pink-50",
    "bg-pink-100",
    "bg-pink-200",
    "bg-pink-300",
    "bg-pink-400",
    "bg-pink-500",
    "bg-pink-600",
    "bg-pink-700",
    "bg-pink-800",
    "bg-pink-900",
  ],
  "background-opacity": [
    "bg-opacity-0",
    "bg-opacity-5",
    "bg-opacity-10",
    "bg-opacity-20",
    "bg-opacity-25",
    "bg-opacity-30",
    "bg-opacity-40",
    "bg-opacity-50",
    "bg-opacity-60",
    "bg-opacity-70",
    "bg-opacity-75",
    "bg-opacity-80",
    "bg-opacity-90",
    "bg-opacity-95",
    "bg-opacity-100",
  ],

  "border-color": [
    "border-transparent",
    "border-current",
    "border-black",
    "border-white",
    "border-gray-100",
    "border-gray-200",
    "border-gray-300",
    "border-gray-400",
    "border-gray-500",
    "border-gray-600",
    "border-gray-700",
    "border-gray-800",
    "border-gray-900",
    "border-red-100",
    "border-red-200",
    "border-red-300",
    "border-red-400",
    "border-red-500",
    "border-red-600",
    "border-red-700",
    "border-red-800",
    "border-red-900",
    "border-orange-100",
    "border-orange-200",
    "border-orange-300",
    "border-orange-400",
    "border-orange-500",
    "border-orange-600",
    "border-orange-700",
    "border-orange-800",
    "border-orange-900",
    "border-yellow-100",
    "border-yellow-200",
    "border-yellow-300",
    "border-yellow-400",
    "border-yellow-500",
    "border-yellow-600",
    "border-yellow-700",
    "border-yellow-800",
    "border-yellow-900",
    "border-green-100",
    "border-green-200",
    "border-green-300",
    "border-green-400",
    "border-green-500",
    "border-green-600",
    "border-green-700",
    "border-green-800",
    "border-green-900",
    "border-teal-100",
    "border-teal-200",
    "border-teal-300",
    "border-teal-400",
    "border-teal-500",
    "border-teal-600",
    "border-teal-700",
    "border-teal-800",
    "border-teal-900",
    "border-blue-100",
    "border-blue-200",
    "border-blue-300",
    "border-blue-400",
    "border-blue-500",
    "border-blue-600",
    "border-blue-700",
    "border-blue-800",
    "border-blue-900",
    "border-indigo-100",
    "border-indigo-200",
    "border-indigo-300",
    "border-indigo-400",
    "border-indigo-500",
    "border-indigo-600",
    "border-indigo-700",
    "border-indigo-800",
    "border-indigo-900",
    "border-purple-100",
    "border-purple-200",
    "border-purple-300",
    "border-purple-400",
    "border-purple-500",
    "border-purple-600",
    "border-purple-700",
    "border-purple-800",
    "border-purple-900",
    "border-pink-100",
    "border-pink-200",
    "border-pink-300",
    "border-pink-400",
    "border-pink-500",
    "border-pink-600",
    "border-pink-700",
    "border-pink-800",
    "border-pink-900",
  ],
  borders: [
    "border-solid",
    "border-dashed",
    "border-dotted",
    "border-double",
    "border-none",
    "border",
    "border-0",
    "border-2",
    "border-4",
    "border-8",
    "divide-x",
    "divide-x-0",
    "divide-x-2",
    "divide-x-4",
    "divide-x-8",
    "divide-y",
    "divide-y-0",
    "divide-y-2",
    "divide-y-4",
    "divide-y-8",
    "border-collapse",
    "border-separate",
    "rounded / rounded-*",
    "border-b",
    "border-b-0",
    "border-blue-50",
    "border-gray-50",
    "border-green-50",
    "border-indigo-50",
    "border-l",
    "border-l-0",
    "border-opacity-0",
    "border-pink-50",
    "border-purple-50",
    "border-r",
    "border-r-0",
    "border-red-50",
    "border-t",
    "border-t-0",
    "border-yellow-50",
    "rounded-2xl",
    "rounded-3xl",
    "rounded-b-2xl",
    "rounded-b-3xl",
    "rounded-b-xl",
    "rounded-bl-2xl",
    "rounded-bl-3xl",
    "rounded-bl-xl",
    "rounded-br-2xl",
    "rounded-br-3xl",
    "rounded-br-xl",
    "rounded-l-2xl",
    "rounded-l-xl",
    "rounded-r-2xl",
    "rounded-r-3xl",
    "rounded-r-xl",
    "rounded-t-2xl",
    "rounded-t-3xl",
    "rounded-t-xl",
    "rounded-tl-2xl",
    "rounded-tl-3xl",
    "rounded-tl-xl",
    "rounded-tr-2xl",
    "rounded-tr-3xl",
    "rounded-tr-xl",
    "rounded-xl",
  ],
  box: ["box-border", "box-content"],
  display: [
    "block",
    "hidden",
    "inline",
    "inline-block",
    "inline-flex",
    "inline-grid",
    "flex",
    "grid",
    "flow-root",
  ],
  flex: [
    "flex-row",
    "flex-row-reverse",
    "flex-col",
    "flex-col-reverse",
    "flex-initial",
    "flex-1",
    "flex-auto",
    "flex-none",
    "flex-no-wrap",
    "flex-wrap",
    "flex-wrap-reverse",
    "items-stretch",
    "justify-start ",
    "self-start",
    "flex-grow",
    "flex-shrink",
    "order-1",
  ],
  flexbox: [
    "content-around",
    "content-between",
    "content-center",
    "content-end",
    "content-evenly",
    "content-start",
    "contents",
    "flex-nowrap",
    "justify-items-auto",
    "justify-items-center",
    "justify-items-end",
    "justify-items-start",
    "justify-items-stretch",
    "justify-self-auto",
    "justify-self-center",
    "justify-self-end",
    "justify-self-start",
    "justify-self-stretch",
    "space-x-7",
    "space-x-px",
    "space-x-reverse",
    "space-y-px",
    "space-y-reverse",
  ],
  height: [
    "h-0",
    "h-1",
    "h-2",
    "h-3",
    "h-4",
    "h-5",
    "h-6",
    "h-8",
    "h-10",
    "h-12",
    "h-16",
    "h-20",
    "h-24",
    "h-32",
    "h-40",
    "h-48",
    "h-56",
    "h-64",
    "h-auto",
    "h-px",
    "h-full",
    "h-screen",
    "max-h-full",
    "max-h-screen",
    "min-h-0",
    "min-h-full",
    "min-h-screen",
    "max-h-px",
  ],
  margins: [
    "m-0",
    "m-1",
    "m-2",
    "m-3",
    "m-4",
    "m-5",
    "m-6",
    "m-8",
    "m-10",
    "m-12",
    "m-16",
    "m-20",
    "m-24",
    "m-32",
    "m-40",
    "m-48",
    "m-56",
    "m-64",
    "m-auto",
    "m-px",
    "mb-0",
    "mb-1",
    "mb-2",
    "mb-3",
    "mb-4",
    "mb-5",
    "mb-6",
    "mb-8",
    "mb-10",
    "mb-12",
    "mb-16",
    "mb-20",
    "mb-24",
    "mb-32",
    "mb-40",
    "mb-48",
    "mb-56",
    "mb-64",
    "mb-auto",
    "mb-px",
    "mr-0",
    "mr-1",
    "mr-2",
    "mr-3",
    "mr-4",
    "mr-5",
    "mr-6",
    "mr-8",
    "mr-10",
    "mr-12",
    "mr-16",
    "mr-20",
    "mr-24",
    "mr-32",
    "mr-40",
    "mr-48",
    "mr-56",
    "mr-64",
    "mr-auto",
    "mr-px",
    "mt-0",
    "mt-1",
    "mt-2",
    "mt-3",
    "mt-4",
    "mt-5",
    "mt-6",
    "mt-8",
    "mt-10",
    "mt-12",
    "mt-16",
    "mt-20",
    "mt-24",
    "mt-32",
    "mt-40",
    "mt-48",
    "mt-56",
    "mt-64",
    "mt-auto",
    "mt-px",
    "ml-0",
    "ml-1",
    "ml-2",
    "ml-3",
    "ml-4",
    "ml-5",
    "ml-6",
    "ml-8",
    "ml-10",
    "ml-12",
    "ml-16",
    "ml-20",
    "ml-24",
    "ml-32",
    "ml-40",
    "ml-48",
    "ml-56",
    "ml-64",
    "ml-auto",
    "ml-px",
    "mx-0",
    "mx-1",
    "mx-2",
    "mx-3",
    "mx-4",
    "mx-5",
    "mx-6",
    "mx-8",
    "mx-10",
    "mx-12",
    "mx-16",
    "mx-20",
    "mx-24",
    "mx-32",
    "mx-40",
    "mx-48",
    "mx-56",
    "mx-64",
    "mx-auto",
    "mx-px",
    "my-0",
    "my-1",
    "my-2",
    "my-3",
    "my-4",
    "my-5",
    "my-6",
    "my-8",
    "my-10",
    "my-12",
    "my-16",
    "my-20",
    "my-24",
    "my-32",
    "my-40",
    "my-48",
    "my-56",
    "my-64",
    "my-auto",
    "my-px",
    "-m-1",
    "-m-2",
    "-m-3",
    "-m-4",
    "-m-5",
    "-m-6",
    "-m-8",
    "-m-10",
    "-m-12",
    "-m-16",
    "-m-20",
    "-m-24",
    "-m-32",
    "-m-40",
    "-m-48",
    "-m-56",
    "-m-64",
    "-m-px",
    "-mb-1",
    "-mb-2",
    "-mb-3",
    "-mb-4",
    "-mb-5",
    "-mb-6",
    "-mb-8",
    "-mb-10",
    "-mb-12",
    "-mb-16",
    "-mb-20",
    "-mb-24",
    "-mb-32",
    "-mb-40",
    "-mb-48",
    "-mb-56",
    "-mb-64",
    "-mb-px",
    "-mr-1",
    "-mr-2",
    "-mr-3",
    "-mr-4",
    "-mr-5",
    "-mr-6",
    "-mr-8",
    "-mr-10",
    "-mr-12",
    "-mr-16",
    "-mr-20",
    "-mr-24",
    "-mr-32",
    "-mr-40",
    "-mr-48",
    "-mr-56",
    "-mr-64",
    "-mr-px",
    "-mt-1",
    "-mt-2",
    "-mt-3",
    "-mt-4",
    "-mt-5",
    "-mt-6",
    "-mt-8",
    "-mt-10",
    "-mt-12",
    "-mt-16",
    "-mt-20",
    "-mt-24",
    "-mt-32",
    "-mt-40",
    "-mt-48",
    "-mt-56",
    "-mt-64",
    "-mt-px",
    "-ml-1",
    "-ml-2",
    "-ml-3",
    "-ml-4",
    "-ml-5",
    "-ml-6",
    "-ml-8",
    "-ml-10",
    "-ml-12",
    "-ml-16",
    "-ml-20",
    "-ml-24",
    "-ml-32",
    "-ml-40",
    "-ml-48",
    "-ml-56",
    "-ml-64",
    "-ml-px",
    "-mx-1",
    "-mx-2",
    "-mx-3",
    "-mx-4",
    "-mx-5",
    "-mx-6",
    "-mx-8",
    "-mx-10",
    "-mx-12",
    "-mx-16",
    "-mx-20",
    "-mx-24",
    "-mx-32",
    "-mx-40",
    "-mx-48",
    "-mx-56",
    "-mx-64",
    "-mx-px",
    "-my-1",
    "-my-2",
    "-my-3",
    "-my-4",
    "-my-5",
    "-my-6",
    "-my-8",
    "-my-10",
    "-my-12",
    "-my-16",
    "-my-20",
    "-my-24",
    "-my-32",
    "-my-40",
    "-my-48",
    "-my-56",
    "-my-64",
    "-my-px",
    "mb-11",
    "ml-11",
    "mr-11",
    "mt-11",
    "mx-11",
    "my-11",
  ],
  paddings: [
    "p-0",
    "p-1",
    "p-2",
    "p-3",
    "p-4",
    "p-5",
    "p-6",
    "p-8",
    "p-10",
    "p-12",
    "p-16",
    "p-20",
    "p-24",
    "p-32",
    "p-40",
    "p-48",
    "p-56",
    "p-64",
    "p-px",
    "pb-0",
    "pb-1",
    "pb-2",
    "pb-3",
    "pb-4",
    "pb-5",
    "pb-6",
    "pb-8",
    "pb-10",
    "pb-12",
    "pb-16",
    "pb-20",
    "pb-24",
    "pb-32",
    "pb-40",
    "pb-48",
    "pb-56",
    "pb-64",
    "pb-px",
    "pr-0",
    "pr-1",
    "pr-2",
    "pr-3",
    "pr-4",
    "pr-5",
    "pr-6",
    "pr-8",
    "pr-10",
    "pr-12",
    "pr-16",
    "pr-20",
    "pr-24",
    "pr-32",
    "pr-40",
    "pr-48",
    "pr-56",
    "pr-64",
    "pr-px",
    "pt-0",
    "pt-1",
    "pt-2",
    "pt-3",
    "pt-4",
    "pt-5",
    "pt-6",
    "pt-8",
    "pt-10",
    "pt-12",
    "pt-16",
    "pt-20",
    "pt-24",
    "pt-32",
    "pt-40",
    "pt-48",
    "pt-56",
    "pt-64",
    "pt-px",
    "pl-0",
    "pl-1",
    "pl-2",
    "pl-3",
    "pl-4",
    "pl-5",
    "pl-6",
    "pl-8",
    "pl-10",
    "pl-12",
    "pl-16",
    "pl-20",
    "pl-24",
    "pl-32",
    "pl-40",
    "pl-48",
    "pl-56",
    "pl-64",
    "pl-px",
    "px-0",
    "px-1",
    "px-2",
    "px-3",
    "px-4",
    "px-5",
    "px-6",
    "px-8",
    "px-10",
    "px-12",
    "px-16",
    "px-20",
    "px-24",
    "px-32",
    "px-40",
    "px-48",
    "px-56",
    "px-64",
    "px-px",
    "py-0",
    "py-1",
    "py-2",
    "py-3",
    "py-4",
    "py-5",
    "py-6",
    "py-8",
    "py-10",
    "py-12",
    "py-16",
    "py-20",
    "py-24",
    "py-32",
    "py-40",
    "py-48",
    "py-56",
    "py-64",
    "py-px",
    "p-11",
    "pb-11",
    "pl-11",
    "pr-11",
    "pt-11",
    "px-11",
    "py-11",
  ],

  shadows: [
    "shadow-xs",
    "shadow-sm",
    "shadow",
    "shadow-md",
    "shadow-lg",
    "shadow-xl",
    "shadow-2xl",
    "shadow-inner",
    "shadow-outline",
    "shadow-none",
  ],
  text: [
    "antialiased",
    "subpixel-antialiased",
    "text-opacity-*",
    "break-normal",
    "break-words",
    "break-all",
    "truncate",
    "uppercase",
    "lowercase",
    "capitalize",
    "normal-case",
    "leading-none",
    "leading-tight",
    "leading-snug",
    "leading-normal",
    "leading-relaxed",
    "leading-loose",
    "leading-3",
    "leading-4",
    "leading-5",
    "leading-6",
    "leading-7",
    "leading-8",
    "leading-9",
    "leading-10",
    "underline",
    "line-through",
    "no-underline",
    "font-hairline",
    "font-thin",
    "font-light",
    "font-normal",
    "font-medium",
    "font-semibold",
    "font-bold",
    "font-extrabold",
    "font-black",
    "text-xs",
    "text-sm",
    "text-base",
    "text-lg",
    "text-xl",
    "text-2xl",
    "text-3xl",
    "text-4xl",
    "text-5xl",
    "text-6xl",
    "font-sans",
    "font-serif",
    "font-mono",
    "text-left",
    "text-center",
    "text-right",
    "text-justify",
    "italic",
    "not-italic",
    "tracking-tighter",
    "tracking-tight",
    "tracking-normal",
    "tracking-wide",
    "tracking-wider",
    "tracking-widest",
  ],
  "text-color": [
    "text-transparent",
    "text-current",
    "text-black",
    "text-white",
    "text-gray-50",
    "text-gray-100",
    "text-gray-200",
    "text-gray-300",
    "text-gray-400",
    "text-gray-500",
    "text-gray-600",
    "text-gray-700",
    "text-gray-800",
    "text-gray-900",
    "text-red-50",
    "text-red-100",
    "text-red-200",
    "text-red-300",
    "text-red-400",
    "text-red-500",
    "text-red-600",
    "text-red-700",
    "text-red-800",
    "text-red-900",
    "text-orange-50",
    "text-orange-100",
    "text-orange-200",
    "text-orange-300",
    "text-orange-400",
    "text-orange-500",
    "text-orange-600",
    "text-orange-700",
    "text-orange-800",
    "text-orange-900",
    "text-yellow-50",
    "text-yellow-100",
    "text-yellow-200",
    "text-yellow-300",
    "text-yellow-400",
    "text-yellow-500",
    "text-yellow-600",
    "text-yellow-700",
    "text-yellow-800",
    "text-yellow-900",
    "text-green-50",
    "text-green-100",
    "text-green-200",
    "text-green-300",
    "text-green-400",
    "text-green-500",
    "text-green-600",
    "text-green-700",
    "text-green-800",
    "text-green-900",
    "text-teal-50",
    "text-teal-100",
    "text-teal-200",
    "text-teal-300",
    "text-teal-400",
    "text-teal-500",
    "text-teal-600",
    "text-teal-700",
    "text-teal-800",
    "text-teal-900",
    "text-blue-50",
    "text-blue-100",
    "text-blue-200",
    "text-blue-300",
    "text-blue-400",
    "text-blue-500",
    "text-blue-600",
    "text-blue-700",
    "text-blue-800",
    "text-blue-900",
    "text-indigo-50",
    "text-indigo-100",
    "text-indigo-200",
    "text-indigo-300",
    "text-indigo-400",
    "text-indigo-500",
    "text-indigo-600",
    "text-indigo-700",
    "text-indigo-800",
    "text-indigo-900",
    "text-purple-50",
    "text-purple-100",
    "text-purple-200",
    "text-purple-300",
    "text-purple-400",
    "text-purple-500",
    "text-purple-600",
    "text-purple-700",
    "text-purple-800",
    "text-purple-900",
    "text-pink-50",
    "text-pink-100",
    "text-pink-200",
    "text-pink-300",
    "text-pink-400",
    "text-pink-500",
    "text-pink-600",
    "text-pink-700",
    "text-pink-800",
    "text-pink-900",
  ],
  visibility: [
    "opacity-100",
    "opacity-75",
    "opacity-50",
    "opacity-25",
    "opacity-0",
    "visible",
    "invisible",
    "sr-only",
    "not-sr-only",
  ],
  width: [
    "w-0",
    "w-1",
    "w-2",
    "w-3",
    "w-4",
    "w-5",
    "w-6",
    "w-8",
    "w-10",
    "w-12",
    "w-16",
    "w-20",
    "w-24",
    "w-32",
    "w-40",
    "w-48",
    "w-56",
    "w-64",
    "w-auto",
    "w-px",
    "w-full",
    "w-screen",
    "w-1/2",
    "w-1/3",
    "w-2/3",
    "w-1/4",
    "w-2/4",
    "w-3/4",
    "w-1/5",
    "w-2/5",
    "w-3/5",
    "w-4/5",
    "w-1/6",
    "w-2/6",
    "w-3/6",
    "w-4/6",
    "w-5/6",
    "w-1/12",
    "w-2/12",
    "w-3/12",
    "w-4/12",
    "w-5/12",
    "w-6/12",
    "w-7/12",
    "w-8/12",
    "w-9/12",
    "w-10/12",
    "w-11/12",
    "max-w-xs",
    "max-w-sm",
    "max-w-md",
    "max-w-lg",
    "max-w-xl",
    "max-w-2xl",
    "max-w-3xl",
    "max-w-4xl",
    "max-w-5xl",
    "max-w-6xl",
    "max-w-full",
    "max-w-screen-sm",
    "max-w-screen-md",
    "max-w-screen-lg",
    "max-w-screen-xl",
    "max-w-none",
    "min-w-0",
    "min-w-full",
    "max-h-0",
    "max-w-0",
    "max-w-7xl",
    "max-w-max",
    "max-w-min",
    "max-w-prose",
    "max-w-screen-2xl",
    "min-w-max",
    "min-w-min",
    "w-11",
  ],
};

let _useableClassesArr: string[] = [];

export const getClasses = (): string[] => {
  if (_useableClassesArr.length > 0) return _useableClassesArr;
  for (const key in useableClasses) {
    const value = useableClasses[key];
    _useableClassesArr = _useableClassesArr.concat(value);
  }

  return _useableClassesArr;
};